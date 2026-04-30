import sys, os, re, json, struct, argparse
from collections import defaultdict

FIFO_TEXCOORD = 0x22
FIFO_BEGIN    = 0x40
FIFO_VERTEX16 = 0x23
FIFO_NOP      = 0x00
GL_TRIANGLES  = 0
GL_QUADS      = 1
VALID_TEX_SIZES = {8, 16, 32, 64, 128, 256, 512, 1024}

def sanitize(name):
    return re.sub(r'[^a-zA-Z0-9_]', '_', name)

def floattov16(f):
    return max(-32768, min(32767, int(f * (1 << 12)))) & 0xFFFF

def floattot16(f):
    return max(-32768, min(32767, int(f * (1 << 4)))) & 0xFFFF

def pack_cmds(c1, c2=FIFO_NOP, c3=FIFO_NOP, c4=FIFO_NOP):
    return struct.pack('<I', (c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

def parse_mtl(mtl_path, extra_mapping=None):
    mapping = {}
    if mtl_path and os.path.exists(mtl_path):
        current = None
        with open(mtl_path, 'r', errors='replace') as f:
            for line in f:
                parts = line.strip().split()
                if not parts: continue
                if parts[0] == 'newmtl':
                    current = parts[1] if len(parts) > 1 else None
                elif parts[0] == 'map_Kd' and current:
                    mapping[current] = line.strip()[len('map_Kd'):].strip()
    if extra_mapping:
        mapping.update(extra_mapping)
    return mapping

def parse_obj(obj_path):
    vertices, texcoords, groups = [], [], []
    current_mat, current_faces = '__no_material__', []

    def flush():
        if current_faces:
            groups.append({'material': current_mat, 'faces': list(current_faces)})

    with open(obj_path, 'r', errors='replace') as f:
        for line in f:
            parts = line.split()
            if not parts: continue
            tok = parts[0]
            if tok == 'v':
                vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif tok == 'vt':
                texcoords.append((float(parts[1]), float(parts[2])))
            elif tok == 'usemtl':
                flush()
                current_mat, current_faces = (parts[1] if len(parts) > 1 else '__no_material__'), []
            elif tok == 'f':
                face = []
                for p in parts[1:]:
                    c = p.split('/')
                    face.append((int(c[0]) - 1, int(c[1]) - 1 if len(c) > 1 and c[1] else None))
                current_faces.append(face)
    flush()
    return vertices, texcoords, groups

def compute_bounds(vertices):
    xs = [v[0] for v in vertices]; ys = [v[1] for v in vertices]; zs = [v[2] for v in vertices]
    return (min(xs), max(xs)), (min(ys), max(ys)), (min(zs), max(zs))

def convert_blender_zup(vertices):
    return [(x, z, y) for x, y, z in vertices]

def build_display_list(faces, vertices, texcoords, scale, offset, tex_w, tex_h, flip_winding=False):
    words = []
    ox, oy, oz = offset
    for face in faces:
        if flip_winding and len(face) >= 2:
            face = [face[1], face[0]] + face[2:]
        n = len(face)
        if n == 4:   prim = GL_QUADS
        elif n == 3: prim = GL_TRIANGLES
        else:
            for i in range(1, n - 1):
                words += build_display_list([face[0], face[i], face[i+1]], vertices, texcoords, scale, offset, tex_w, tex_h, flip_winding)
            continue
        words.append(pack_cmds(FIFO_BEGIN))
        words.append(struct.pack('<I', prim))
        for vi, vti in face:
            if vti is not None and tex_w and tex_h:
                u, v = texcoords[vti]
                u16 = floattot16(u * tex_w)
                v16 = floattot16((1.0 - v) * tex_h)
                words.append(pack_cmds(FIFO_TEXCOORD))
                words.append(struct.pack('<I', (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16)))
            vx, vy, vz = vertices[vi]
            sx = (vx - ox) * scale; sy = (vy - oy) * scale; sz = (vz - oz) * scale
            words.append(pack_cmds(FIFO_VERTEX16))
            words.append(struct.pack('<I', (floattov16(sy) << 16) | floattov16(sx)))
            words.append(struct.pack('<I', floattov16(sz)))
    return words

def find_texture_size(png_path):
    try:
        with open(png_path, 'rb') as f:
            f.read(16)
            w = struct.unpack('>I', f.read(4))[0]
            h = struct.unpack('>I', f.read(4))[0]
            return w, h
    except Exception:
        return None, None

def nearest_valid_tex_size(n):
    if n is None: return None
    for s in sorted(VALID_TEX_SIZES):
        if s >= n: return s
    return 1024

def convert(obj_path, output_dir, config):
    scale = config.get("scale", None)
    target_size = config.get("target_size", 4.0)
    
    center = config.get("center", True)
    if config.get("no_center"):
        center = False
        
    blender_source = config.get("source_blender", False)
    
    extra_mapping = None
    if config.get("mapping"):
        try:
            with open(config["mapping"]) as f:
                extra_mapping = json.load(f)
        except Exception:
            pass

    obj_path   = os.path.abspath(obj_path)
    output_dir = os.path.abspath(output_dir)
    os.makedirs(output_dir, exist_ok=True)

    obj_dir   = os.path.dirname(obj_path)
    base_name = sanitize(os.path.splitext(os.path.basename(obj_path))[0])

    mtl_path = None
    with open(obj_path, 'r', errors='replace') as f:
        for line in f:
            parts = line.strip().split()
            if parts and parts[0] == 'mtllib':
                mtl_path = os.path.join(obj_dir, ' '.join(parts[1:]))
                break
    mat_to_tex = parse_mtl(mtl_path, extra_mapping)

    print(f"Parsing {os.path.basename(obj_path)} ...")
    vertices, texcoords, groups = parse_obj(obj_path)
    print(f"  {len(vertices)} vertices, {sum(len(g['faces']) for g in groups)} faces, {len(groups)} material groups")

    if blender_source:
        print("  Converting Blender Z-up → NDS Y-up")
        vertices = convert_blender_zup(vertices)

    (xmin, xmax), (ymin, ymax), (zmin, zmax) = compute_bounds(vertices)
    max_dim = max(xmax - xmin, ymax - ymin, zmax - zmin)

    if scale is None:
        scale = (target_size / max_dim) if max_dim > 0 else 1.0
        print(f"  Auto-scale: {scale:.6f}  (target {target_size} NDS units, model {max_dim:.3f})")
    else:
        print(f"  Manual scale: {scale:.6f}")

    if center:
        ox = (xmin + xmax) / 2.0; oy = ymin; oz = (zmin + zmax) / 2.0
    else:
        ox = oy = oz = 0.0
    offset = (ox, oy, oz)
    print(f"  Center offset: ({ox:.3f}, {oy:.3f}, {oz:.3f})")

    trans_min_x = (xmin - ox) * scale; trans_max_x = (xmax - ox) * scale
    trans_min_z = (zmin - oz) * scale; trans_max_z = (zmax - oz) * scale
    world_offset_x = -trans_min_x
    world_offset_z = -trans_min_z
    world_width    = trans_max_x - trans_min_x
    world_depth    = trans_max_z - trans_min_z

    merged, tex_paths = defaultdict(list), {}
    for g in groups:
        mat, faces = g['material'], g['faces']
        if not faces: continue
        tex_rel = mat_to_tex.get(mat)
        if tex_rel is None:
            print(f"  WARNING: no texture for '{mat}' — rendering without UV")
            tex_key = '__no_texture__'
        else:
            tex_abs = os.path.join(obj_dir, tex_rel)
            tex_key = os.path.basename(tex_rel)
            tex_paths[tex_key] = tex_abs
        merged[tex_key].extend(faces)

    print(f"  Texture groups: {len(merged)}")

    dl_groups = []
    for tex_key, faces in merged.items():
        tex_abs = tex_paths.get(tex_key)
        tw, th  = find_texture_size(tex_abs) if tex_abs else (None, None)
        if tw: tw = nearest_valid_tex_size(tw)
        if th: th = nearest_valid_tex_size(th)
        words = build_display_list(faces, vertices, texcoords, scale, offset, tw, th, flip_winding=blender_source)
        dl_groups.append((tex_key, words, tw, th))
        print(f"  [{tex_key}] {len(faces)} faces, {tw}x{th}, {len(words)} words")

    header_path   = os.path.join(output_dir, f'{base_name}_env.h')
    s_path        = os.path.join(output_dir, f'{base_name}_env.s')
    tex_list_path = os.path.join(output_dir, f'{base_name}_textures.txt')
    n = len(dl_groups)

    with open(s_path, 'w') as s:
        s.write(f"@ Auto-generated by obj2environment.py\n")
        s.write(f"\t.section .rodata\n")
        s.write(f"\t.align 2\n\n")
        for i, (tex_key, words, tw, th) in enumerate(dl_groups):
            dl_name = f"{base_name}_dl_{i}"
            s.write(f"\t.global {dl_name}\n")
            s.write(f"{dl_name}:\n")
            s.write(f"\t.word {len(words)} @ count\n")
            for j in range(0, len(words), 8):
                chunk = words[j:j+8]
                vals = [f"0x{struct.unpack('<I', w)[0]:08X}" for w in chunk]
                s.write(f"\t.word " + ", ".join(vals) + "\n")
            s.write("\n")
    print(f"Wrote: {s_path}")

    with open(header_path, 'w') as h:
        h.write(f"#pragma once\n// Auto-generated by obj2environment.py\n")
        h.write(f"// Source: {os.path.basename(obj_path)}\n")
        h.write(f"// Scale: {scale:.6f}  Centred: {center}\n")
        h.write(f"// DO NOT EDIT — regenerate from source.\n\n#include <nds.h>\n\n")
        h.write("#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")
        for i in range(n):
            h.write(f"extern const u32 {base_name}_dl_{i}[];\n")
        h.write("\n#ifdef __cplusplus\n}\n#endif\n\n")
        h.write("// --- World Bounds ---\n")
        h.write(f"#define {base_name.upper()}_WORLD_OFFSET_X {world_offset_x:.6f}f\n"
                f"#define {base_name.upper()}_WORLD_OFFSET_Z {world_offset_z:.6f}f\n"
                f"#define {base_name.upper()}_WORLD_WIDTH    {world_width:.6f}f\n"
                f"#define {base_name.upper()}_WORLD_DEPTH    {world_depth:.6f}f\n\n")

        h.write(f"enum {base_name}_TexSlot {{\n")
        for i, (tex_key, _, _, _) in enumerate(dl_groups):
            h.write(f"    {base_name.upper()}_TEX_{sanitize(os.path.splitext(tex_key)[0]).upper()} = {i},\n")
        h.write(f"    {base_name.upper()}_TEX_COUNT = {n}\n}};\n\n")

        h.write(f"static const int {base_name}_tex_widths[{n}]  = {{\n    " + ', '.join(str(tw or 0) for _, _, tw, _ in dl_groups) + "\n};\n")
        h.write(f"static const int {base_name}_tex_heights[{n}] = {{\n    " + ', '.join(str(th or 0) for _, _, _, th in dl_groups) + "\n};\n\n")

        h.write(f"inline void Load_{base_name}(int ids[{n}], const unsigned int* bitmaps[{n}]) {{\n")
        for i, (tex_key, _, tw, th) in enumerate(dl_groups):
            nw = f"TEXTURE_SIZE_{tw}" if tw else "TEXTURE_SIZE_8"
            nh = f"TEXTURE_SIZE_{th}" if th else "TEXTURE_SIZE_8"
            h.write(f"    glGenTextures(1, &ids[{i}]);\n"
                    f"    glBindTexture(GL_TEXTURE_2D, ids[{i}]);\n"
                    f"    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, {nw}, {nh}, 0,\n"
                    f"        TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,\n"
                    f"        bitmaps[{i}]);\n")
        h.write("}\n\n")

        h.write(f"inline void Draw_{base_name}(const int ids[{n}]) {{\n")
        for i, (tex_key, _, _, _) in enumerate(dl_groups):
            h.write(f"    glBindTexture(GL_TEXTURE_2D, ids[{i}]);  // {tex_key}\n"
                    f"    glCallList((u32*){base_name}_dl_{i});\n")
        h.write("}\n\n")

        h.write(f"inline void Delete_{base_name}(int ids[{n}]) {{\n    glDeleteTextures({n}, ids);\n}}\n")

    print(f"Wrote: {header_path}")

    with open(tex_list_path, 'w') as t:
        t.write(f"# Textures for {base_name} — run GRIT on each\n\n")
        for i, (tex_key, _, tw, th) in enumerate(dl_groups):
            abs_path = tex_paths.get(tex_key, f"<missing: {tex_key}>")
            t.write(f"# Slot {i}  ({tw}x{th})  var: {sanitize(os.path.splitext(tex_key)[0])}Bitmap\n{abs_path}\n\n")
    print(f"Wrote: {tex_list_path}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert a multi-texture OBJ to an NDS C header.')
    parser.add_argument('input')
    parser.add_argument('output_dir')
    parser.add_argument('--scale',          type=float, default=None)
    parser.add_argument('--target-size',    type=float, default=4.0)
    parser.add_argument('--no-center',      action='store_true')
    parser.add_argument('--source-blender', action='store_true')
    parser.add_argument('--mapping',        type=str,   default=None)
    args = parser.parse_args()

    cli_config = {
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender,
        "mapping": args.mapping
    }
    convert(args.input, args.output_dir, cli_config)