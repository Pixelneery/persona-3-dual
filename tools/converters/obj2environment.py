import sys, os, re, json, struct, argparse
from collections import defaultdict

FIFO_COLOR    = 0x20
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
    return int(f * (1 << 4)) & 0xFFFF

def to_signed_v16(f):
    return max(-32768, min(32767, int(f * (1 << 12))))

def to_signed_t16(f):
    return int(f * (1 << 4))

def rgb_to_rgb15(r, g, b):
    return ((r >> 3) & 0x1F) | (((g >> 3) & 0x1F) << 5) | (((b >> 3) & 0x1F) << 10)

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
    is_billboard = False

    def flush():
        if current_faces:
            groups.append({
                'material': current_mat, 
                'faces': list(current_faces),
                'is_billboard': is_billboard
            })

    with open(obj_path, 'r', errors='replace') as f:
        for line in f:
            parts = line.split()
            if not parts: continue
            tok = parts[0]
            if tok == 'v':
                vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif tok == 'vt':
                texcoords.append((float(parts[1]), float(parts[2])))
            elif tok in ('o', 'g'):
                flush()
                is_billboard = parts[1].startswith('BB_') if len(parts) > 1 else False
                current_faces = []
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
    if not vertices: return (0,0), (0,0), (0,0)
    xs = [v[0] for v in vertices]; ys = [v[1] for v in vertices]; zs = [v[2] for v in vertices]
    return (min(xs), max(xs)), (min(ys), max(ys)), (min(zs), max(zs))

def convert_blender_zup(vertices):
    return [(x, z, y) for x, y, z in vertices]

def build_display_list(faces, vertices, texcoords, scale, offset, tex_w, tex_h, flip_winding=False, blender_source=False, vertex_color=None):
    words = []
    ox, oy, oz = offset

    if vertex_color is not None:
        r, g, b = vertex_color
        words.append(pack_cmds(FIFO_COLOR))
        words.append(struct.pack('<I', rgb_to_rgb15(r, g, b)))

    triangles, quads = [], []
    for face in faces:
        if flip_winding and len(face) >= 2:
            face = face[::-1] # Safe quad winding

        if len(face) == 3: triangles.append(face)
        elif len(face) == 4: quads.append(face)
        else:
            for i in range(1, len(face) - 1):
                triangles.append([face[0], face[i], face[i + 1]])

    def emit_faces(face_list, prim_type):
        if not face_list: return
        words.append(pack_cmds(FIFO_BEGIN))
        words.append(struct.pack('<I', prim_type))

        for face in face_list:
            for vi, vti in face:
                if vti is not None and tex_w and tex_h:
                    u, v_orig = texcoords[vti]
                    v = (1.0 - v_orig) if blender_source else v_orig
                    u16 = floattot16(u * tex_w)
                    v16 = floattot16(v * tex_h)
                    words.append(pack_cmds(FIFO_TEXCOORD))
                    words.append(struct.pack('<I', (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16)))
                vx, vy, vz = vertices[vi]
                sx = (vx - ox) * scale; sy = (vy - oy) * scale; sz = (vz - oz) * scale
                words.append(pack_cmds(FIFO_VERTEX16))
                words.append(struct.pack('<I', (floattov16(sy) << 16) | floattov16(sx)))
                words.append(struct.pack('<I', floattov16(sz)))

    emit_faces(quads, GL_QUADS)
    emit_faces(triangles, GL_TRIANGLES)
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
    if config.get("no_center"): center = False
    blender_source = config.get("source_blender", False)
    
    rgba_all = config.get("rgba", False)
    raw_rgba_list = config.get("rgba_list", [])
    if isinstance(raw_rgba_list, str): rgba_list = [x.strip() for x in raw_rgba_list.split(',') if x.strip()]
    elif isinstance(raw_rgba_list, list): rgba_list = [str(x).strip() for x in raw_rgba_list]
    else: rgba_list = []
        
    vertex_color = config.get("color", [255, 255, 255])
    if isinstance(vertex_color, list) and len(vertex_color) == 3: vertex_color = tuple(vertex_color)
    elif vertex_color is None: vertex_color = (255, 255, 255)
    
    extra_mapping = None
    if config.get("mapping"):
        try:
            with open(config["mapping"]) as f: extra_mapping = json.load(f)
        except Exception: pass

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

    vertices, texcoords, groups = parse_obj(obj_path)

    if blender_source: vertices = convert_blender_zup(vertices)

    (xmin, xmax), (ymin, ymax), (zmin, zmax) = compute_bounds(vertices)
    max_dim = max(xmax - xmin, ymax - ymin, zmax - zmin)
    if scale is None: scale = (target_size / max_dim) if max_dim > 0 else 1.0
    
    if center: ox = (xmin + xmax) / 2.0; oy = ymin; oz = (zmin + zmax) / 2.0
    else: ox = oy = oz = 0.0
    offset = (ox, oy, oz)

    trans_min_x = (xmin - ox) * scale; trans_max_x = (xmax - ox) * scale
    trans_min_z = (zmin - oz) * scale; trans_max_z = (zmax - oz) * scale
    world_offset_x = -trans_min_x; world_offset_z = -trans_min_z
    world_width    = trans_max_x - trans_min_x; world_depth    = trans_max_z - trans_min_z

    merged, tex_paths = defaultdict(list), {}
    billboards = []

    for g in groups:
        mat, faces = g['material'], g['faces']
        if not faces: continue
        tex_rel = mat_to_tex.get(mat)
        if tex_rel is None:
            tex_key = '__no_texture__'
        else:
            tex_abs = os.path.join(obj_dir, tex_rel)
            tex_key = os.path.basename(tex_rel)
            tex_paths[tex_key] = tex_abs
        
        if g.get('is_billboard'):
            v_idx, vt_idx = set(), set()
            for face in faces:
                for vi, vti in face:
                    v_idx.add(vi)
                    if vti is not None: vt_idx.add(vti)
            
            b_verts = [vertices[i] for i in v_idx]
            (b_xmin, b_xmax), (b_ymin, b_ymax), (b_zmin, b_zmax) = compute_bounds(b_verts)
            
            cx = ((b_xmin + b_xmax) / 2.0 - ox) * scale
            cy = ((b_ymin + b_ymax) / 2.0 - oy) * scale
            cz = ((b_zmin + b_zmax) / 2.0 - oz) * scale
            
            hw = (max(b_xmax - b_xmin, b_zmax - b_zmin) / 2.0) * scale
            hh = ((b_ymax - b_ymin) / 2.0) * scale

            u_min, u_max, v_min, v_max = 0.0, 1.0, 0.0, 1.0
            if vt_idx:
                b_uvs = [texcoords[i] for i in vt_idx]
                u_min, u_max = min(uv[0] for uv in b_uvs), max(uv[0] for uv in b_uvs)
                v_min_raw, v_max_raw = min(uv[1] for uv in b_uvs), max(uv[1] for uv in b_uvs)
                if blender_source: v_min, v_max = 1.0 - v_max_raw, 1.0 - v_min_raw
                else: v_min, v_max = v_min_raw, v_max_raw

            billboards.append({
                'cx': cx, 'cy': cy, 'cz': cz, 'hw': hw, 'hh': hh,
                'tex_key': tex_key, 'u_min': u_min, 'u_max': u_max, 'v_min': v_min, 'v_max': v_max
            })
            if tex_key not in merged: merged[tex_key] = []
        else:
            merged[tex_key].extend(faces)

    dl_groups = []
    tex_to_slot = {}
    
    for tex_key, faces in merged.items():
        tex_abs = tex_paths.get(tex_key)
        tw, th  = find_texture_size(tex_abs) if tex_abs else (None, None)
        
        if tw is None or th is None:
            if tex_key != '__no_texture__':
                print(f"\n[WARNING] Could not find or read texture image: {tex_abs}")
                print(f"-> Defaulting to 8x8. Check your file path!")
            tw, th = 8, 8
            
        tw, th = nearest_valid_tex_size(tw), nearest_valid_tex_size(th)
        words = build_display_list(faces, vertices, texcoords, scale, offset, tw, th, flip_winding=blender_source, blender_source=blender_source, vertex_color=vertex_color)
        
        tex_to_slot[tex_key] = len(dl_groups)
        dl_groups.append((tex_key, words, tw, th))

    for b in billboards:
        b['slot'] = tex_to_slot[b['tex_key']]
        tw, th = dl_groups[b['slot']][2] or 8, dl_groups[b['slot']][3] or 8
        b['u0_16'] = to_signed_t16(b['u_min'] * tw)
        b['v0_16'] = to_signed_t16(b['v_min'] * th)
        b['u1_16'] = to_signed_t16(b['u_max'] * tw)
        b['v1_16'] = to_signed_t16(b['v_max'] * th)

    billboards.sort(key=lambda b: b['slot'])

    bin_path      = os.path.join(output_dir, f'{base_name}.bin')
    header_path   = os.path.join(output_dir, f'{base_name}.h')
    tex_list_path = os.path.join(output_dir, f'{base_name}_textures.txt')
    n = len(dl_groups); safe_n = max(n, 1)

    with open(bin_path, 'wb') as f:
        f.write(b'ENV1')
        f.write(struct.pack('<I', n))
        for _, words, _, _ in dl_groups:
            f.write(struct.pack('<I', len(words)))
            for w in words: f.write(w)
    print(f"Wrote: {bin_path}")

    with open(header_path, 'w') as h:
        h.write(f"#pragma once\n// Auto-generated by obj2environment.py\n")
        h.write(f"// Source: {os.path.basename(obj_path)}\n")
        h.write(f"// Scale: {scale:.6f}  Centred: {center}\n")
        h.write(f"// DO NOT EDIT - regenerate from source.\n\n")
        h.write(f"#include <nds.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <math.h>\n\n")
        
        h.write("// world Bounds\n")
        h.write(f"#define {base_name.upper()}_WORLD_OFFSET_X {world_offset_x:.6f}f\n"
                f"#define {base_name.upper()}_WORLD_OFFSET_Z {world_offset_z:.6f}f\n"
                f"#define {base_name.upper()}_WORLD_WIDTH    {world_width:.6f}f\n"
                f"#define {base_name.upper()}_WORLD_DEPTH    {world_depth:.6f}f\n\n")

        h.write(f"enum {base_name}_TexSlot {{\n")
        for i, (tex_key, _, _, _) in enumerate(dl_groups):
            h.write(f"    {base_name.upper()}_TEX_{sanitize(os.path.splitext(tex_key)[0]).upper()} = {i},\n")
        h.write(f"    {base_name.upper()}_TEX_COUNT = {n}\n}};\n\n")

        h.write(f"struct {base_name}_BillboardData {{\n")
        h.write(f"    v16 x, y, z;\n")
        h.write(f"    v16 halfWidth, halfHeight;\n")
        h.write(f"    int texSlot;\n")
        h.write(f"    short u0, v0, u1, v1;\n")
        h.write(f"}};\n\n")

        h.write(f"class {base_name}_Environment {{\npublic:\n")
        h.write(f"    u32* displayLists[{safe_n}];\n")
        h.write(f"    u32 dlSizes[{safe_n}];\n")
        h.write(f"    int textureIDs[{safe_n}];\n\n")
        
        h.write(f"    static const int BILLBOARD_COUNT = {len(billboards)};\n")
        if billboards:
            h.write(f"    const {base_name}_BillboardData BILLBOARDS[{len(billboards)}] = {{\n")
            for b in billboards:
                h.write(f"        {{ {to_signed_v16(b['cx'])}, {to_signed_v16(b['cy'])}, {to_signed_v16(b['cz'])}, "
                        f"{to_signed_v16(b['hw'])}, {to_signed_v16(b['hh'])}, {b['slot']}, "
                        f"{b['u0_16']}, {b['v0_16']}, {b['u1_16']}, {b['v1_16']} }},\n")
            h.write(f"    }};\n\n")
        else:
            h.write(f"    const {base_name}_BillboardData* BILLBOARDS = NULL;\n\n")
        
        h.write(f"    {base_name}_Environment() {{\n")
        h.write(f"        for (int i = 0; i < {safe_n}; i++) {{\n")
        h.write(f"            displayLists[i] = NULL; dlSizes[i] = 0; textureIDs[i] = 0;\n")
        h.write(f"        }}\n    }}\n\n")

        h.write(f"    bool load(const char* filepath, const unsigned int* bitmaps[{safe_n}]) {{\n")
        if n > 0:
            h.write(f"        FILE* file = fopen(filepath, \"rb\");\n")
            h.write(f"        if (!file) return false;\n\n")
            h.write(f"        char magic[4];\n        fread(magic, 1, 4, file);\n")
            h.write(f"        if (magic[0] != 'E' || magic[1] != 'N' || magic[2] != 'V' || magic[3] != '1') {{\n")
            h.write(f"            fclose(file); return false;\n        }}\n\n")
            h.write(f"        u32 groupCount;\n        fread(&groupCount, sizeof(u32), 1, file);\n")
            h.write(f"        if (groupCount != {n}) {{ fclose(file); return false; }}\n\n")
            h.write(f"        for (u32 i = 0; i < groupCount; i++) {{\n")
            h.write(f"            fread(&dlSizes[i], sizeof(u32), 1, file);\n")
            h.write(f"            displayLists[i] = (u32*)malloc((dlSizes[i] + 1) * sizeof(u32));\n")
            h.write(f"            displayLists[i][0] = dlSizes[i];\n")
            h.write(f"            if (dlSizes[i] > 0) fread(&displayLists[i][1], sizeof(u32), dlSizes[i], file);\n")
            h.write(f"        }}\n        fclose(file);\n\n")
            
            # FIX: Dynamically restores proper TEXTURE_SIZE formatting
            for i, (tex_key, _, tw, th) in enumerate(dl_groups):
                nw = f"TEXTURE_SIZE_{tw}" if tw else "TEXTURE_SIZE_8"
                nh = f"TEXTURE_SIZE_{th}" if th else "TEXTURE_SIZE_8"
                is_rgba = rgba_all or any(req in tex_key for req in rgba_list)
                gl_format = "GL_RGBA" if is_rgba else "GL_RGB"
                
                h.write(f"        if (bitmaps[{i}]) {{\n")
                h.write(f"            glGenTextures(1, &textureIDs[{i}]);\n")
                h.write(f"            glBindTexture(GL_TEXTURE_2D, textureIDs[{i}]);\n")
                h.write(f"            glTexImage2D(GL_TEXTURE_2D, 0, {gl_format}, {nw}, {nh}, 0, TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T, bitmaps[{i}]);\n")
                h.write(f"        }}\n")
            h.write(f"        return true;\n")
        else:
            h.write("        return true;\n")
        h.write("    }\n\n")

        h.write(f"    void draw() {{\n")
        for i, (tex_key, _, _, _) in enumerate(dl_groups):
            h.write(f"        glBindTexture(GL_TEXTURE_2D, textureIDs[{i}]);\n")
            h.write(f"        if (displayLists[{i}]) glCallList(displayLists[{i}]);\n")
        h.write("    }\n\n")

        # FIX: True Viewpoint (Spherical/Cylindrical) Billboarding!
        h.write(f"    void drawBillboards(bool faceCamera, float camX, float camY, float camZ) {{\n")
        h.write(f"        if (BILLBOARD_COUNT == 0) return;\n")
        h.write(f"        int currentSlot = -1;\n")
        h.write(f"        bool inQuads = false;\n\n")
        
        h.write(f"        for (int i = 0; i < BILLBOARD_COUNT; i++) {{\n")
        h.write(f"            const {base_name}_BillboardData& bb = BILLBOARDS[i];\n")
        h.write(f"            if (bb.texSlot != currentSlot) {{\n")
        h.write(f"                if (inQuads) {{ glEnd(); inQuads = false; }}\n")
        h.write(f"                glBindTexture(GL_TEXTURE_2D, textureIDs[bb.texSlot]);\n")
        h.write(f"                currentSlot = bb.texSlot;\n")
        h.write(f"            }}\n")
        h.write(f"            if (!inQuads) {{ glBegin(GL_QUADS); inQuads = true; }}\n\n")
        
        # Default static right vector if tracking is disabled
        h.write(f"            v16 rX = (v16)(4096), rY = 0, rZ = 0;\n")
        h.write(f"            v16 uX = 0, uY = (v16)(4096), uZ = 0;\n\n")
        
        # Calculate exactly how THIS specific billboard needs to rotate to look at the camera
        h.write(f"            if (faceCamera) {{\n")
        h.write(f"                float bx = (float)bb.x / 4096.0f;\n")
        h.write(f"                float bz = (float)bb.z / 4096.0f;\n")
        h.write(f"                float dx = camX - bx;\n")
        h.write(f"                float dz = camZ - bz;\n")
        h.write(f"                float dist = sqrt(dx*dx + dz*dz);\n")
        h.write(f"                if (dist > 0.001f) {{ dx /= dist; dz /= dist; }}\n")
        h.write(f"                rX = (v16)(dz * 4096.0f);\n")
        h.write(f"                rZ = (v16)(-dx * 4096.0f);\n")
        h.write(f"            }}\n\n")

        h.write(f"            v16 rx = mulf32(rX, bb.halfWidth);\n")
        h.write(f"            v16 ry = mulf32(rY, bb.halfWidth);\n")
        h.write(f"            v16 rz = mulf32(rZ, bb.halfWidth);\n")
        h.write(f"            v16 ux = mulf32(uX, bb.halfHeight);\n")
        h.write(f"            v16 uy = mulf32(uY, bb.halfHeight);\n")
        h.write(f"            v16 uz = mulf32(uZ, bb.halfHeight);\n\n")
        
        h.write(f"            glTexCoord2t16(bb.u0, bb.v1);\n")
        h.write(f"            glVertex3v16(bb.x - rx - ux, bb.y - ry - uy, bb.z - rz - uz);\n")
        h.write(f"            glTexCoord2t16(bb.u1, bb.v1);\n")
        h.write(f"            glVertex3v16(bb.x + rx - ux, bb.y + ry - uy, bb.z + rz - uz);\n")
        h.write(f"            glTexCoord2t16(bb.u1, bb.v0);\n")
        h.write(f"            glVertex3v16(bb.x + rx + ux, bb.y + ry + uy, bb.z + rz + uz);\n")
        h.write(f"            glTexCoord2t16(bb.u0, bb.v0);\n")
        h.write(f"            glVertex3v16(bb.x - rx + ux, bb.y - ry + uy, bb.z - rz + uz);\n")
        h.write(f"        }}\n")
        h.write(f"        if (inQuads) glEnd();\n")
        h.write(f"    }}\n\n")

        h.write(f"    void cleanup() {{\n")
        if n > 0:
            h.write(f"        for (u32 i = 0; i < {n}; i++) {{\n")
            h.write(f"            if (displayLists[i]) {{ free(displayLists[i]); displayLists[i] = NULL; }}\n")
            h.write(f"        }}\n        glDeleteTextures({n}, textureIDs);\n")
        h.write("    }\n};\n")

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
    parser.add_argument('--rgba',           action='store_true', help='Use GL_RGBA for all textures.')
    parser.add_argument('--rgba-list',      type=str,   default='', help='Comma-separated list of texture names to use GL_RGBA.')
    parser.add_argument('--color',          nargs=3,    type=int, metavar=('R', 'G', 'B'), default=[255, 255, 255])
    args = parser.parse_args()

    cli_config = {
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender,
        "mapping": args.mapping,
        "rgba": args.rgba,
        "rgba_list": args.rgba_list,
        "color": args.color
    }
    convert(args.input, args.output_dir, cli_config)