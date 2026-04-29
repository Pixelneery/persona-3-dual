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

VALID_SIZES = {8, 16, 32, 64, 128, 256, 512, 1024}

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

def create_display_list(input_file, tex_width=None, tex_height=None, vertex_color=None):
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

    has_uvs = any(vt is not None for face in faces for _, vt in face)
    words = []

    if vertex_color is not None:
        r, g, b = vertex_color
        words.append(pack_cmds(FIFO_COLOR))
        words.append(struct.pack('<I', rgb_to_rgb15(r, g, b)))

    for face in faces:
        if len(face) == 4: prim_type = GL_QUADS
        elif len(face) == 3: prim_type = GL_TRIANGLES
        else: continue

        words.append(pack_cmds(FIFO_BEGIN))
        words.append(struct.pack('<I', prim_type))

        for v_idx, vt_idx in face:
            if has_uvs and vt_idx is not None:
                u, v = texcoords[vt_idx]
                u16   = floattot16(u * tex_width)
                v16   = floattot16((1.0 - v) * tex_height)
                words.append(pack_cmds(FIFO_TEXCOORD))
                words.append(struct.pack('<I', (u16 & 0xFFFF) | ((v16 & 0xFFFF) << 16)))

            vert = vertices[v_idx]
            words.append(pack_cmds(FIFO_VERTEX16))
            words.append(struct.pack('<I', (floattov16(vert[1]) << 16) | floattov16(vert[0])))
            words.append(struct.pack('<I', floattov16(vert[2])))

    return words

def convert_obj(input_file, output_file, tex_width=None, tex_height=None, vertex_color=None):
    words = create_display_list(input_file, tex_width, tex_height, vertex_color)
    with open(output_file, 'wb') as out:
        out.write(struct.pack('<I', len(words)))
        for w in words: out.write(w)

def convert_model_json(input_file, output_file, tex_width=None, tex_height=None, vertex_color=None):
    base_dir = os.path.dirname(input_file)
    
    # Strip off the _WxH if it exists so the enums and functions are clean (e.g., player_64x64 -> player)
    raw_name = os.path.splitext(os.path.basename(input_file))[0].replace('-', '_')
    if 'x' in raw_name:
        parts = raw_name.rsplit('_', 1)
        if len(parts) > 1 and 'x' in parts[1]:
            raw_name = parts[0]
            
    model_name = raw_name
    
    with open(input_file, 'r') as f:
        data = json.load(f)
    
    header_out = output_file.replace('.bin', '.h')
    
    with open(header_out, 'w') as out:
        out.write(f"#pragma once\n#include <nds.h>\n#include \"controllers/AnimationController.h\"\n\n")
        
        # --- GENERATE ENUMS FOR TYPE SAFETY ---
        out.write(f"// Auto-generated Model Enums\n")
        out.write(f"enum Model_{model_name} {{\n")
        anim_names = list(data['animations'].keys())
        for i, anim_name in enumerate(anim_names):
            out.write(f"    MODEL_{model_name.upper()}_{anim_name.upper()} = {i},\n")
        out.write("};\n\n")

        node_count = len(data['nodes'])
        
        # 1. Compile Display Lists
        for node in data['nodes']:
            obj_path = os.path.join(base_dir, node['obj'])
            words = create_display_list(obj_path, tex_width, tex_height, vertex_color)
            out.write(f"static const u32 {model_name}_dl_{node['id']}[] = {{\n    ")
            out.write(f"{len(words)}, ") 
            for w in words:
                val = struct.unpack('<I', w)[0]
                out.write(f"0x{val:08X}, ")
            out.write("\n};\n\n")

        # 2. Compile Keyframes
        for anim_name, anim_data in data['animations'].items():
            for node_id, keyframes in anim_data['tracks'].items():
                out.write(f"static const Keyframe {model_name}_{anim_name}_n{node_id}[] = {{\n")
                for kf in keyframes:
                    rx = to_s16((kf['rot'][0] / 360.0) * 32768)
                    ry = to_s16((kf['rot'][1] / 360.0) * 32768)
                    rz = to_s16((kf['rot'][2] / 360.0) * 32768)
                    px = to_s16(floattov16(kf['pos'][0] / 16.0))
                    py = to_s16(floattov16(kf['pos'][1] / 16.0))
                    pz = to_s16(floattov16(kf['pos'][2] / 16.0))
                    out.write(f"    {{{kf['time']}, {rx}, {ry}, {rz}, {px}, {py}, {pz}}},\n")
                out.write("};\n")

        # 3. LOADER FUNCTION
        out.write(f"\ninline void LoadModel_{model_name}(AnimationController& ctrl) {{\n")
        out.write(f"    std::vector<AnimNode> nodes({node_count});\n")
        
        for node in data['nodes']:
            nid = node['id']
            pid = node['parent']
            origin = node.get('origin', [0, 0, 0])
            ox = to_s16(floattov16(origin[0] / 16.0))
            oy = to_s16(floattov16(origin[1] / 16.0))
            oz = to_s16(floattov16(origin[2] / 16.0))

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

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert OBJ to NDS display list binary')
    parser.add_argument('input',  help='Input .obj or .json file')
    parser.add_argument('output', help='Output .bin file')
    parser.add_argument('--texsize', nargs=2, type=int, metavar=('W', 'H'))
    parser.add_argument('--color', nargs=3, type=int, metavar=('R', 'G', 'B'))
    args = parser.parse_args()

    tw, th = (args.texsize[0], args.texsize[1]) if args.texsize else (None, None)
    color  = tuple(args.color) if args.color else None

    if args.input.endswith('.json'):
        convert_model_json(args.input, args.output, tex_width=tw, tex_height=th, vertex_color=color)
    else:
        convert_obj(args.input, args.output, tex_width=tw, tex_height=th, vertex_color=color)
        print(f"Converted {args.input} -> {args.output}")