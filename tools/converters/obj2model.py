import sys
import struct
import argparse
import json
import os

FIFO_COLOR    = 0x20
FIFO_TEXCOORD = 0x22
FIFO_BEGIN    = 0x40
FIFO_VERTEX16 = 0x23
FIFO_NOP      = 0x00
GL_TRIANGLES = 0
GL_QUADS     = 1

def floattov16(f):
    v = int(f * (1 << 12))
    return max(-32768, min(32767, v)) & 0xFFFF

def floattot16(f):
    v = int(f * (1 << 4))
    return max(-32768, min(32767, v)) & 0xFFFF

def rgb_to_rgb15(r, g, b):
    return ((r >> 3) & 0x1F) | (((g >> 3) & 0x1F) << 5) | (((b >> 3) & 0x1F) << 10)

def pack_cmds(c1, c2=FIFO_NOP, c3=FIFO_NOP, c4=FIFO_NOP):
    return struct.pack('<I', (c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

def to_s16(val):
    val = int(val) & 0xFFFF
    return val if val <= 32767 else val - 65536

def parse_obj(input_file):
    vertices = []
    texcoords = []
    faces = []
    with open(input_file, 'r') as f:
        for line in f:
            parts = line.split()
            if not parts: continue
            if parts[0] == 'v':
                vertices.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif parts[0] == 'vt':
                texcoords.append((float(parts[1]), float(parts[2])))
            elif parts[0] == 'f':
                face = []
                for p in parts[1:]:
                    components = p.split('/')
                    v_idx  = int(components[0]) - 1
                    vt_idx = int(components[1]) - 1 if len(components) > 1 and components[1] != '' else None
                    face.append((v_idx, vt_idx))
                faces.append(face)
    return vertices, texcoords, faces

def build_display_list(vertices, texcoords, faces, tex_width, tex_height, vertex_color, scale, offset, blender_source):
    has_uvs = any(vt is not None for face in faces for _, vt in face)
    words = []

    if vertex_color is not None:
        r, g, b = vertex_color
        words.append(pack_cmds(FIFO_COLOR))
        words.append(struct.pack('<I', rgb_to_rgb15(r, g, b)))

    ox, oy, oz = offset

    for face in faces:
        if blender_source and len(face) >= 2:
            face = [face[1], face[0]] + face[2:]
            
        if len(face) == 4: prim_type = GL_QUADS
        elif len(face) == 3: prim_type = GL_TRIANGLES
        else: continue

        words.append(pack_cmds(FIFO_BEGIN))
        words.append(struct.pack('<I', prim_type))

        for v_idx, vt_idx in face:
            if has_uvs and vt_idx is not None and tex_width and tex_height:
                u, v = texcoords[vt_idx]
                u16   = floattot16(u * tex_width)
                v16   = floattot16((1.0 - v) * tex_height)
                words.append(pack_cmds(FIFO_TEXCOORD))
                words.append(struct.pack('<I', (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16)))

            vx, vy, vz = vertices[v_idx]
            sx = (vx - ox) * scale
            sy = (vy - oy) * scale
            sz = (vz - oz) * scale
            
            words.append(pack_cmds(FIFO_VERTEX16))
            words.append(struct.pack('<I', (floattov16(sy) << 16) | floattov16(sx)))
            words.append(struct.pack('<I', floattov16(sz)))

    return words

def convert_obj(input_file, output_file, tex_width=None, tex_height=None, vertex_color=None, scale=None, target_size=4.0, center=True, blender_source=False):
    v, vt, f = parse_obj(input_file)
    
    if blender_source:
        v = [(x, z, y) for x, y, z in v]
        
    xs = [pt[0] for pt in v]
    ys = [pt[1] for pt in v]
    zs = [pt[2] for pt in v]
    
    max_dim = max(max(xs)-min(xs), max(ys)-min(ys), max(zs)-min(zs)) if v else 1.0
    
    if scale is None:
        scale = (target_size / max_dim) if max_dim > 0 else 1.0
        
    if center and v:
        offset = ((min(xs)+max(xs))/2.0, min(ys), (min(zs)+max(zs))/2.0)
    else:
        offset = (0.0, 0.0, 0.0)

    words = build_display_list(v, vt, f, tex_width, tex_height, vertex_color, scale, offset, blender_source)
    with open(output_file, 'wb') as out:
        out.write(struct.pack('<I', len(words)))
        for w in words: out.write(w)
        
    return output_file

def convert_model_json(input_file, output_file, tex_width=None, tex_height=None, vertex_color=None, scale=None, target_size=4.0, center=True, blender_source=False):
    base_dir = os.path.dirname(input_file)
    model_name = os.path.splitext(os.path.basename(input_file))[0].replace('-', '_')
    
    with open(input_file, 'r') as f:
        data = json.load(f)
    
    header_out = output_file if output_file.endswith('.h') else os.path.splitext(output_file)[0] + '.h'
    
    all_verts = []
    obj_data = {}
    for node in data['nodes']:
        obj_path = os.path.join(base_dir, node['obj'])
        v, vt, f = parse_obj(obj_path)
        if blender_source:
            v = [(x, z, y) for x, y, z in v]
        obj_data[node['id']] = (v, vt, f)
        all_verts.extend(v)

    if not all_verts:
        all_verts = [(0,0,0)]
        
    xs = [pt[0] for pt in all_verts]
    ys = [pt[1] for pt in all_verts]
    zs = [pt[2] for pt in all_verts]
    
    max_dim = max(max(xs)-min(xs), max(ys)-min(ys), max(zs)-min(zs))
    if scale is None:
        scale = (target_size / max_dim) if max_dim > 0 else 1.0
        print(f"  Auto-scale: {scale:.6f}  (target {target_size}, max_dim {max_dim:.3f})")
    else:
        print(f"  Manual scale: {scale:.6f}")
        
    if center:
        offset = ((min(xs)+max(xs))/2.0, min(ys), (min(zs)+max(zs))/2.0)
    else:
        offset = (0.0, 0.0, 0.0)
    print(f"  Center offset: {offset}")

    with open(header_out, 'w') as out:
        out.write(f"#pragma once\n#include <nds.h>\n#include \"controllers/AnimationController.h\"\n\n")
        out.write(f"// Auto-generated Model Enums\n")
        out.write(f"enum Model_{model_name} {{\n")
        anim_names = list(data['animations'].keys())
        for i, anim_name in enumerate(anim_names):
            out.write(f"    MODEL_{model_name.upper()}_{anim_name.upper()} = {i},\n")
        out.write("};\n\n")

        node_count = len(data['nodes'])
        
        for node in data['nodes']:
            nid = node['id']
            v, vt, f = obj_data[nid]
            words = build_display_list(v, vt, f, tex_width, tex_height, vertex_color, scale, offset, blender_source)
            out.write(f"static const u32 {model_name}_dl_{nid}[] = {{\n    ")
            out.write(f"{len(words)}, ") 
            for w in words:
                val = struct.unpack('<I', w)[0]
                out.write(f"0x{val:08X}, ")
            out.write("\n};\n\n")

        for anim_name, anim_data in data['animations'].items():
            for node_id, keyframes in anim_data['tracks'].items():
                out.write(f"static const Keyframe {model_name}_{anim_name}_n{node_id}[] = {{\n")
                for kf in keyframes:
                    rot = kf.get('rot', [0,0,0])
                    pos = kf.get('pos', [0,0,0])
                    
                    if blender_source:
                        rot = [rot[0], rot[2], rot[1]]
                        pos = [pos[0], pos[2], pos[1]]

                    rx = to_s16((rot[0] / 360.0) * 32768)
                    ry = to_s16((rot[1] / 360.0) * 32768)
                    rz = to_s16((rot[2] / 360.0) * 32768)
                    
                    # Blockbench JSON is 16x larger than its OBJ geometry. Blender is 1:1.
                    bb_scale = 1.0 if blender_source else 16.0
                    
                    # Scale positions (no offset because keyframe pos is a delta local transform)
                    px = to_s16(floattov16((pos[0] / bb_scale) * scale))
                    py = to_s16(floattov16((pos[1] / bb_scale) * scale))
                    pz = to_s16(floattov16((pos[2] / bb_scale) * scale))
                    
                    out.write(f"    {{{kf['time']}, {rx}, {ry}, {rz}, {px}, {py}, {pz}}},\n")
                out.write("};\n")

        out.write(f"\ninline void LoadModel_{model_name}(AnimationController& ctrl) {{\n")
        out.write(f"    std::vector<AnimNode> nodes({node_count});\n")
        
        for node in data['nodes']:
            nid = node['id']
            pid = node['parent']
            origin = node.get('origin', [0, 0, 0])
            
            if blender_source:
                origin = [origin[0], origin[2], origin[1]]
                
            # Blockbench JSON is 16x larger than its OBJ geometry. Blender is 1:1.
            bb_scale = 1.0 if blender_source else 16.0
                
            # Origin is absolute position in armature space, so apply offset AND scale
            ox = to_s16(floattov16(((origin[0] / bb_scale) - offset[0]) * scale))
            oy = to_s16(floattov16(((origin[1] / bb_scale) - offset[1]) * scale))
            oz = to_s16(floattov16(((origin[2] / bb_scale) - offset[2]) * scale))

            out.write(f"    nodes[{nid}].id = {nid};\n")
            out.write(f"    nodes[{nid}].parentId = {pid};\n")
            out.write(f"    nodes[{nid}].displayList = (u32*){model_name}_dl_{nid};\n")
            out.write(f"    nodes[{nid}].displayListSize = {model_name}_dl_{nid}[0];\n")
            out.write(f"    nodes[{nid}].pivotX = {ox};\n")
            out.write(f"    nodes[{nid}].pivotY = {oy};\n")
            out.write(f"    nodes[{nid}].pivotZ = {oz};\n")

        out.write(f"    std::vector<Animation> anims;\n    Animation a;\n    AnimTrack t;\n")
        for anim_name, anim_data in data['animations'].items():
            out.write(f"    a.name = \"{anim_name}\";\n")
            out.write(f"    a.duration = {anim_data['duration']};\n")
            out.write(f"    a.nodeTracks.assign({node_count}, AnimTrack());\n")
            for node_id, keyframes in anim_data['tracks'].items():
                kf_array = f"{model_name}_{anim_name}_n{node_id}"
                kf_count = len(keyframes)
                out.write(f"    t.frames.assign({kf_array}, {kf_array} + {kf_count});\n")
                out.write(f"    a.nodeTracks[{node_id}] = t;\n")
            out.write(f"    anims.push_back(a);\n")

        out.write(f"    ctrl.loadModel(nodes, anims);\n}}\n")
        
    return header_out

def convert(input_file, output_file, config):
    tex_w, tex_h = None, None
    texsize = config.get("texsize")
    if isinstance(texsize, list) and len(texsize) >= 2:
        tex_w, tex_h = texsize[0], texsize[1]
    elif isinstance(texsize, str):
        parts = texsize.split()
        if len(parts) >= 2:
            tex_w, tex_h = int(parts[0]), int(parts[1])

    vertex_color = config.get("color")
    if isinstance(vertex_color, list) and len(vertex_color) == 3:
        vertex_color = tuple(vertex_color)

    scale = config.get("scale")
    target_size = config.get("target_size", 4.0)
    
    center = config.get("center", True)
    if config.get("no_center"):
        center = False

    blender_source = config.get("source_blender", False)

    if input_file.endswith('.json'):
        final_out = convert_model_json(input_file, output_file, tex_width=tex_w, tex_height=tex_h, vertex_color=vertex_color, scale=scale, target_size=target_size, center=center, blender_source=blender_source)
    else:
        final_out = convert_obj(input_file, output_file, tex_width=tex_w, tex_height=tex_h, vertex_color=vertex_color, scale=scale, target_size=target_size, center=center, blender_source=blender_source)
        
    print(f"Converted {input_file} -> {final_out}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert OBJ/JSON to NDS display list')
    parser.add_argument('input', help='Input .obj or .json file')
    parser.add_argument('output', help='Output .bin or .h file')
    parser.add_argument('--texsize', nargs=2, type=int, metavar=('W', 'H'))
    parser.add_argument('--color', nargs=3, type=int, metavar=('R', 'G', 'B'))
    parser.add_argument('--scale', type=float, default=None)
    parser.add_argument('--target-size', type=float, default=4.0)
    parser.add_argument('--no-center', action='store_true')
    parser.add_argument('--source-blender', action='store_true')
    args = parser.parse_args()

    cli_config = {
        "texsize": args.texsize,
        "color": args.color,
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender
    }
    convert(args.input, args.output, cli_config)