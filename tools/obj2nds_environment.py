#!/usr/bin/env python3
"""
obj2nds_environment.py  —  Multi-texture OBJ -> NDS C Header Converter
For the Persona 3 Dual project.

Reads a standard OBJ + MTL file pair (as exported by Blender) and produces:
  - modelName_env.h        : C++ header with display lists + load/draw functions
  - modelName_textures.txt : list of PNG files that need to be converted with GRIT

The MTL file MUST have `map_Kd <filename>` entries for each material.
Blender exports these by default when you assign an Image Texture to a material.

Usage:
    python obj2nds_environment.py input.obj output_dir/
    python obj2nds_environment.py input.obj output_dir/ --scale 0.054
    python obj2nds_environment.py input.obj output_dir/ --target-size 4.0
    python obj2nds_environment.py input.obj output_dir/ --scale 0.054 --no-center
    python obj2nds_environment.py input.obj output_dir/ --tiles 0 0 64 64

Options:
    --scale FLOAT       Explicit scale factor applied to all vertex coordinates.
    --target-size FLOAT Auto-compute scale so the longest model dimension equals
                        this value in NDS units. Default: 4.0.
                        Ignored if --scale is given.
    --no-center         Do not translate the model to be centred at the origin.
                        By default the model is centred on X/Z and sits on Y=0.
    --mapping FILE      JSON file mapping material names to PNG paths, for cases
                        where the MTL has no map_Kd entries (e.g. XPS exports).
                        Format: {"MaterialName": "path/to/texture.png", ...}
    --tiles X Z W H     Injects world bounds & collision macros into the header.
                        Takes 4 values: start_x, start_z, columns, rows.
"""

import sys
import os
import re
import json
import struct
import argparse
from collections import defaultdict

# ── NDS GPU constants ──────────────────────────────────────────────────────────
FIFO_COLOR    = 0x20
FIFO_TEXCOORD = 0x22
FIFO_BEGIN    = 0x40
FIFO_VERTEX16 = 0x23
FIFO_NOP      = 0x00
GL_TRIANGLES  = 0
GL_QUADS      = 1

VALID_TEX_SIZES = {8, 16, 32, 64, 128, 256, 512, 1024}

# ── Fixed-point helpers ────────────────────────────────────────────────────────

def floattov16(f):
    v = int(f * (1 << 12))
    return max(-32768, min(32767, v)) & 0xFFFF

def floattot16(f):
    v = int(f * (1 << 4))
    return max(-32768, min(32767, v)) & 0xFFFF

def pack_cmds(c1, c2=FIFO_NOP, c3=FIFO_NOP, c4=FIFO_NOP):
    return struct.pack('<I', (c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

def sanitize(name):
    return re.sub(r'[^a-zA-Z0-9_]', '_', name)

# ── OBJ / MTL parser ──────────────────────────────────────────────────────────

def parse_mtl(mtl_path, extra_mapping=None):
    """
    Returns dict: material_name -> png_filename (or None if not found).
    Parses `map_Kd` lines. extra_mapping overrides/supplements MTL entries.
    """
    mapping = {}
    if mtl_path and os.path.exists(mtl_path):
        current = None
        with open(mtl_path, 'r', errors='replace') as f:
            for line in f:
                parts = line.strip().split()
                if not parts:
                    continue
                if parts[0] == 'newmtl':
                    current = parts[1] if len(parts) > 1 else None
                elif parts[0] == 'map_Kd' and current is not None:
                    # path may contain spaces — take everything after the keyword
                    tex_path = line.strip()[len('map_Kd'):].strip()
                    mapping[current] = tex_path
    if extra_mapping:
        mapping.update(extra_mapping)
    return mapping


def parse_obj(obj_path):
    """
    Returns:
        vertices  : list of (x, y, z)
        texcoords : list of (u, v)
        groups    : list of {'material': str, 'faces': [[(v_idx, vt_idx), ...]]}
    """
    vertices  = []
    texcoords = []
    groups    = []
    current_mat = '__no_material__'
    current_faces = []

    def flush():
        if current_faces:
            groups.append({'material': current_mat, 'faces': list(current_faces)})

    with open(obj_path, 'r', errors='replace') as f:
        for line in f:
            parts = line.split()
            if not parts:
                continue
            tok = parts[0]
            if tok == 'v':
                vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif tok == 'vt':
                texcoords.append((float(parts[1]), float(parts[2])))
            elif tok == 'usemtl':
                flush()
                current_mat   = parts[1] if len(parts) > 1 else '__no_material__'
                current_faces = []
            elif tok == 'f':
                face = []
                for p in parts[1:]:
                    comps = p.split('/')
                    vi  = int(comps[0]) - 1
                    vti = int(comps[1]) - 1 if len(comps) > 1 and comps[1] else None
                    face.append((vi, vti))
                current_faces.append(face)

    flush()
    return vertices, texcoords, groups


# ── Geometry helpers ───────────────────────────────────────────────────────────

def compute_bounds(vertices):
    xs = [v[0] for v in vertices]
    ys = [v[1] for v in vertices]
    zs = [v[2] for v in vertices]
    return (min(xs), max(xs)), (min(ys), max(ys)), (min(zs), max(zs))


def build_display_list(faces, vertices, texcoords, scale, offset, tex_w, tex_h, flip_winding=False):
    """
    Converts a list of faces into NDS FIFO words.
    offset = (ox, oy, oz) subtracted from each vertex before scaling.
    Returns list of bytes objects (each 4 bytes).
    """
    words = []
    ox, oy, oz = offset

    for face in faces:
        if flip_winding and len(face) >= 2:
            face = [face[1], face[0]] + face[2:]

        n = len(face)
        if n == 4:
            prim = GL_QUADS
        elif n == 3:
            prim = GL_TRIANGLES
        else:
            # Triangulate n-gon as a fan
            for i in range(1, n - 1):
                sub = [face[0], face[i], face[i + 1]]
                words += build_display_list([sub], vertices, texcoords,
                                             scale, offset, tex_w, tex_h,
                                             flip_winding=flip_winding)
            continue

        words.append(pack_cmds(FIFO_BEGIN))
        words.append(struct.pack('<I', prim))

        for vi, vti in face:
            # UV
            if vti is not None and tex_w and tex_h:
                u, v = texcoords[vti]
                u16  = floattot16(u * tex_w)
                v16  = floattot16((1.0 - v) * tex_h)
                words.append(pack_cmds(FIFO_TEXCOORD))
                words.append(struct.pack('<I', (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16)))

            # Vertex
            vx, vy, vz = vertices[vi]
            sx = (vx - ox) * scale
            sy = (vy - oy) * scale
            sz = (vz - oz) * scale
            words.append(pack_cmds(FIFO_VERTEX16))
            words.append(struct.pack('<I', (floattov16(sy) << 16) | floattov16(sx)))
            words.append(struct.pack('<I', floattov16(sz)))

    return words


# ── Header generation ──────────────────────────────────────────────────────────

def words_to_c_array(words):
    """Format a list of bytes objects as a C hex literal list."""
    hex_vals = [f'0x{struct.unpack("<I", w)[0]:08X}' for w in words]
    # 8 per line
    lines = []
    for i in range(0, len(hex_vals), 8):
        lines.append('    ' + ', '.join(hex_vals[i:i+8]) + ',')
    return '\n'.join(lines)


def find_texture_size(png_path):
    """Return (w, h) of a PNG file, or (None, None) on failure."""
    try:
        from PIL import Image
        img = Image.open(png_path)
        return img.size  # (width, height)
    except Exception:
        pass
    # Fallback: read PNG header manually (bytes 16-24)
    try:
        with open(png_path, 'rb') as f:
            f.read(16)
            w = struct.unpack('>I', f.read(4))[0]
            h = struct.unpack('>I', f.read(4))[0]
            return w, h
    except Exception:
        return None, None


def nearest_valid_tex_size(n):
    if n is None:
        return None
    for s in sorted(VALID_TEX_SIZES):
        if s >= n:
            return s
    return 1024


def convert_blender_zup_vertices(vertices):
    """Convert Blender Z-up vertices to NDS Y-up vertices.
    Blender exports using X right, Y forward, Z up. The NDS pipeline expects
    X right, Y up, Z forward.
    """
    return [(x, z, y) for x, y, z in vertices]


# ── Main ───────────────────────────────────────────────────────────────────────

def convert(obj_path, output_dir, scale=None, target_size=4.0,
            center=True, extra_mapping=None, tiles=None, blender_source=False):

    obj_path   = os.path.abspath(obj_path)
    output_dir = os.path.abspath(output_dir)
    os.makedirs(output_dir, exist_ok=True)

    obj_dir    = os.path.dirname(obj_path)
    base_name  = sanitize(os.path.splitext(os.path.basename(obj_path))[0])

    # ── Find MTL ──────────────────────────────────────────────────────────────
    mtl_path = None
    with open(obj_path, 'r', errors='replace') as f:
        for line in f:
            parts = line.strip().split()
            if parts and parts[0] == 'mtllib':
                mtl_name = ' '.join(parts[1:])
                mtl_path = os.path.join(obj_dir, mtl_name)
                break

    mat_to_tex = parse_mtl(mtl_path, extra_mapping)

    # ── Parse geometry ────────────────────────────────────────────────────────
    print(f"Parsing {os.path.basename(obj_path)} ...")
    vertices, texcoords, groups = parse_obj(obj_path)
    print(f"  {len(vertices)} vertices, {sum(len(g['faces']) for g in groups)} faces, "
          f"{len(groups)} material groups")

    if blender_source:
        print("  Converting from Blender Z-up coordinates to NDS Y-up coordinates...")
        vertices = convert_blender_zup_vertices(vertices)

    # ── Compute scale ─────────────────────────────────────────────────────────
    (xmin, xmax), (ymin, ymax), (zmin, zmax) = compute_bounds(vertices)
    max_dim = max(xmax - xmin, ymax - ymin, zmax - zmin)

    if scale is None:
        if max_dim > 0:
            scale = target_size / max_dim
        else:
            scale = 1.0
        print(f"  Auto-scale: {scale:.6f}  "
              f"(target size {target_size} NDS units, model max dim {max_dim:.3f})")
    else:
        print(f"  Manual scale: {scale:.6f}")

    # ── Compute centering offset ───────────────────────────────────────────────
    if center:
        # Centre on X/Z, sit on Y=0
        ox = (xmin + xmax) / 2.0
        oy = ymin
        oz = (zmin + zmax) / 2.0
    else:
        ox, oy, oz = 0.0, 0.0, 0.0

    offset = (ox, oy, oz)
    print(f"  Centering offset: ({ox:.3f}, {oy:.3f}, {oz:.3f})")

    # ── Compute transformed bounds for Collision ──────────────────────────────
    trans_min_x = (xmin - ox) * scale
    trans_max_x = (xmax - ox) * scale
    trans_min_z = (zmin - oz) * scale
    trans_max_z = (zmax - oz) * scale

    base_world_offset_x = -trans_min_x
    base_world_offset_z = -trans_min_z
    world_width         = trans_max_x - trans_min_x
    world_depth         = trans_max_z - trans_min_z

    # ── Merge groups that share the same texture ───────────────────────────────
    # Key: resolved texture filename (basename)
    merged = defaultdict(list)   # tex_basename -> [face_list, ...]
    mat_tex_resolved = {}        # material_name -> tex_basename

    obj_dir_abs = os.path.dirname(obj_path)
    tex_paths   = {}             # tex_basename -> abs path

    for g in groups:
        mat   = g['material']
        faces = g['faces']
        if not faces:
            continue

        tex_rel = mat_to_tex.get(mat)
        if tex_rel is None:
            print(f"  WARNING: no texture mapping for material '{mat}' — "
                  f"faces will be rendered without UV (solid colour).")
            tex_key = '__no_texture__'
        else:
            # Resolve path relative to OBJ location
            tex_abs = os.path.join(obj_dir_abs, tex_rel)
            tex_key = os.path.basename(tex_rel)
            tex_paths[tex_key] = tex_abs

        merged[tex_key].extend(faces)
        mat_tex_resolved[mat] = tex_key

    print(f"  Texture groups after merge: {len(merged)}")

    # ── Build display lists ───────────────────────────────────────────────────
    # Ordered list of (tex_key, words)
    dl_groups = []  # (tex_key, words)

    for tex_key, faces in merged.items():
        # Detect texture size
        tex_abs = tex_paths.get(tex_key)
        tw, th  = find_texture_size(tex_abs) if tex_abs else (None, None)
        if tw and th:
            tw = nearest_valid_tex_size(tw)
            th = nearest_valid_tex_size(th)

        words = build_display_list(faces, vertices, texcoords,
                                    scale, offset, tw, th,
                                    flip_winding=blender_source)
        dl_groups.append((tex_key, words, tw, th))
        print(f"  [{tex_key}] {len(faces)} faces, "
              f"tex {tw}x{th}, {len(words)} FIFO words")

    # ── Write C header ────────────────────────────────────────────────────────
    header_path = os.path.join(output_dir, f'{base_name}_env.h')
    tex_list_path = os.path.join(output_dir, f'{base_name}_textures.txt')

    with open(header_path, 'w') as h:
        h.write(f"#pragma once\n")
        h.write(f"// Auto-generated by obj2nds_environment.py\n")
        h.write(f"// Source: {os.path.basename(obj_path)}\n")
        h.write(f"// Scale:  {scale:.6f}  |  Centred: {center}\n")
        h.write(f"// DO NOT EDIT — regenerate from source instead.\n\n")
        h.write(f"#include <nds.h>\n\n")

        # ── Injected Collision & World Bounds ─────────────────────────
        h.write(f"// --- World Bounds & Collision ---\n")
        if tiles and len(tiles) == 4:
            start_x, start_z, cols, rows = tiles
            tile_size_x = world_width / cols if cols else 0.0
            tile_size_z = world_depth / rows if rows else 0.0

            # Shift the base offset by the manual start coordinates (in grid tiles)
            final_offset_x = base_world_offset_x - (start_x * tile_size_x)
            final_offset_z = base_world_offset_z - (start_z * tile_size_z)

            h.write(f"#define {base_name.upper()}_WORLD_OFFSET_X {final_offset_x:.6f}f\n")
            h.write(f"#define {base_name.upper()}_WORLD_OFFSET_Z {final_offset_z:.6f}f\n")
            h.write(f"#define {base_name.upper()}_TILE_SIZE_X    {tile_size_x:.6f}f\n")
            h.write(f"#define {base_name.upper()}_TILE_SIZE_Z    {tile_size_z:.6f}f\n")
            h.write(f"#define {base_name.upper()}_MAP_COLS       {int(cols)}\n")
            h.write(f"#define {base_name.upper()}_MAP_ROWS       {int(rows)}\n")
        else:
            h.write(f"#define {base_name.upper()}_WORLD_OFFSET_X {base_world_offset_x:.6f}f\n")
            h.write(f"#define {base_name.upper()}_WORLD_OFFSET_Z {base_world_offset_z:.6f}f\n")
            h.write(f"#define {base_name.upper()}_WORLD_WIDTH    {world_width:.6f}f\n")
            h.write(f"#define {base_name.upper()}_WORLD_DEPTH    {world_depth:.6f}f\n")
        h.write(f"\n")

        n_groups = len(dl_groups)

        # Texture index enum
        h.write(f"// Texture slot indices — use these to index into the textureIds array\n")
        h.write(f"enum {base_name}_TexSlot {{\n")
        for i, (tex_key, _, _, _) in enumerate(dl_groups):
            slot_name = sanitize(os.path.splitext(tex_key)[0]).upper()
            h.write(f"    {base_name.upper()}_TEX_{slot_name} = {i},\n")
        h.write(f"    {base_name.upper()}_TEX_COUNT = {n_groups}\n")
        h.write(f"}};\n\n")

        # Display list arrays
        for i, (tex_key, words, tw, th) in enumerate(dl_groups):
            arr_name = f"{base_name}_dl_{i}"
            slot_name = sanitize(os.path.splitext(tex_key)[0]).upper()
            h.write(f"// Display list for texture slot {i}: {tex_key}  ({tw}x{th})\n")
            h.write(f"static const u32 {arr_name}[] = {{\n")
            h.write(f"    {len(words)},  // word count\n")
            h.write(words_to_c_array(words))
            h.write(f"\n}};\n\n")

        # Texture size table — used by LoadEnvironment to call glTexImage2D
        h.write(f"// Per-slot texture dimensions (NDS GL_TEXTURE_SIZE_ENUM values)\n")
        h.write(f"// These match the {base_name}_TexSlot enum order.\n")
        h.write(f"static const int {base_name}_tex_widths[{n_groups}]  = {{\n    ")
        h.write(', '.join(str(tw if tw else 0) for _, _, tw, _ in dl_groups))
        h.write(f"\n}};\n")
        h.write(f"static const int {base_name}_tex_heights[{n_groups}] = {{\n    ")
        h.write(', '.join(str(th if th else 0) for _, _, _, th in dl_groups))
        h.write(f"\n}};\n\n")

        # LoadEnvironment function
        h.write(f"// Call once during scene Init() after setting up VRAM.\n")
        h.write(f"// Pass in your pre-allocated textureIds array (size {base_name.upper()}_TEX_COUNT).\n")
        h.write(f"// Pass in your bitmap arrays in slot order (same order as {base_name}_TexSlot).\n")
        h.write(f"inline void Load_{base_name}(int textureIds[{n_groups}], const unsigned int* bitmaps[{n_groups}]) {{\n")
        for i, (tex_key, _, tw, th) in enumerate(dl_groups):
            slot_name = sanitize(os.path.splitext(tex_key)[0])
            nds_w = f"TEXTURE_SIZE_{tw}" if tw else "TEXTURE_SIZE_8"
            nds_h = f"TEXTURE_SIZE_{th}" if th else "TEXTURE_SIZE_8"
            h.write(f"    // Slot {i}: {tex_key}\n")
            h.write(f"    glGenTextures(1, &textureIds[{i}]);\n")
            h.write(f"    glBindTexture(GL_TEXTURE_2D, textureIds[{i}]);\n")
            h.write(f"    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,\n")
            h.write(f"        {nds_w}, {nds_h}, 0,\n")
            h.write(f"        TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,\n")
            h.write(f"        bitmaps[{i}]);\n\n")
        h.write(f"}}\n\n")

        # DrawEnvironment function
        h.write(f"// Call each frame inside your Draw/Update function.\n")
        h.write(f"// textureIds must be the same array passed to Load_{base_name}.\n")
        h.write(f"inline void Draw_{base_name}(const int textureIds[{n_groups}]) {{\n")
        for i, (tex_key, _, _, _) in enumerate(dl_groups):
            h.write(f"    glBindTexture(GL_TEXTURE_2D, textureIds[{i}]);  // {tex_key}\n")
            h.write(f"    glCallList((u32*){base_name}_dl_{i});\n")
        h.write(f"}}\n\n")

        # DeleteEnvironment function
        h.write(f"// Call during scene Cleanup().\n")
        h.write(f"inline void Delete_{base_name}(int textureIds[{n_groups}]) {{\n")
        h.write(f"    glDeleteTextures({n_groups}, textureIds);\n")
        h.write(f"}}\n")

    print(f"\nWrote: {header_path}")

    # ── Write texture file list ────────────────────────────────────────────────
    with open(tex_list_path, 'w') as t:
        t.write(f"# Textures required for {base_name}\n")
        t.write(f"# Run GRIT on each of these files to produce .h + .s pairs\n")
        t.write(f"# Suggested GRIT flags: -ftb -fh -gb -gB16 -pu16\n\n")
        for i, (tex_key, _, tw, th) in enumerate(dl_groups):
            abs_path = tex_paths.get(tex_key, f"<missing: {tex_key}>")
            var_name = sanitize(os.path.splitext(tex_key)[0]) + 'Bitmap'
            t.write(f"# Slot {i}  ({tw}x{th})  variable: {var_name}\n")
            t.write(f"{abs_path}\n\n")

    print(f"Wrote: {tex_list_path}")

    # ── Print usage summary ────────────────────────────────────────────────────
    print(f"""
─────────────────────────────────────────────────────────────
Next steps:

1. Convert each texture PNG with GRIT:
   grit texture.png -ftc -fh -gb -gB16 -pu16
   (Do this for all {n_groups} textures listed in {os.path.basename(tex_list_path)})

2. In your NDS project, include the generated header and bitmaps:

   #include "{base_name}_env.h"
   // Include GRIT-generated headers for each texture, e.g.:
""")
    for i, (tex_key, _, _, _) in enumerate(dl_groups):
        var_name = sanitize(os.path.splitext(tex_key)[0])
        print(f"   #include \"{var_name}.h\"")

    print(f"""
3. In your View class, declare:
   static int {base_name}TextureIds[{base_name.upper()}_TEX_COUNT];

4. In Init():
   const unsigned int* bitmaps[{base_name.upper()}_TEX_COUNT] = {{""")
    for i, (tex_key, _, _, _) in enumerate(dl_groups):
        var_name = sanitize(os.path.splitext(tex_key)[0])
        print(f"       {var_name}Bitmap,")
    print(f"   }};")
    print(f"   Load_{base_name}({base_name}TextureIds, bitmaps);")
    print(f"""
5. In your Draw/Update():
   glPushMatrix();
     Draw_{base_name}({base_name}TextureIds);
   glPopMatrix(1);

6. In Cleanup():
   Delete_{base_name}({base_name}TextureIds);
─────────────────────────────────────────────────────────────""")

    return header_path, tex_list_path


# ── Entry point ────────────────────────────────────────────────────────────────

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Convert a multi-texture OBJ to NDS display list C header.'
    )
    parser.add_argument('input',        help='Input .obj file')
    parser.add_argument('output_dir',   help='Output directory')
    parser.add_argument('--scale',      type=float, default=None,
                        help='Explicit scale factor (overrides --target-size)')
    parser.add_argument('--target-size',type=float, default=4.0,
                        help='Auto-scale so longest axis = this many NDS units (default: 4.0)')
    parser.add_argument('--no-center',  action='store_true',
                        help='Do not translate model to origin')
    parser.add_argument('--mapping',    type=str, default=None,
                        help='JSON file: {"MaterialName": "texture.png"} for MTLs with no map_Kd')
    parser.add_argument('--tiles',      type=float, nargs=4, metavar=('START_X', 'START_Z', 'COLS', 'ROWS'),
                        default=None,
                        help='Injects collision macros into header (e.g. --tiles 0 0 64 64)')
    parser.add_argument('--source-blender', action='store_true',
                        help='Treat the input OBJ as Blender Z-up coordinates and convert to NDS Y-up')
    args = parser.parse_args()

    extra = None
    if args.mapping:
        with open(args.mapping) as f:
            extra = json.load(f)

    convert(
        args.input,
        args.output_dir,
        scale=args.scale,
        target_size=args.target_size,
        center=not args.no_center,
        extra_mapping=extra,
        tiles=args.tiles,
        blender_source=args.source_blender,
    )