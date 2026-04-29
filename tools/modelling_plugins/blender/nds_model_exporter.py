"""
NDS Model Exporter for Blender
Author: Taha Rashid
Version: 1.0.0
Blender: 3.x / 4.x

Exports a rig to the same ZIP format as the Blockbench plugin:
  - modelName_nX.obj  (one per bone, geometry skinned to that bone)
  - modelName.json    (hierarchy + animation data)

The output ZIP is fully compatible with the existing Python converter
(obj2nds.py / convert_model_json).

------- HOW TO USE -------
1.  Install: Edit > Preferences > Add-ons > Install > pick this file > enable it.
2.  Set up your scene:
    - Use an ARMATURE with MESH children (or a mesh parented to specific bones
      via vertex groups named after the bones).
    - Texture must be a single image in the active material of your mesh(es).
3.  File > Export > NDS Model (.zip)
4.  Choose output path and click Export NDS Model.

------- CONVENTIONS -------
- One bone  =  one NDS node.
- Geometry is assigned to a bone via vertex groups (group name == bone name).
  Vertices with no group, or groups that don't match a bone, are ignored.
- Bone parent chain maps directly to NDS parentId (-1 = root).
- Armature rest pose = bind pose used for static geometry export.
- Animations: all NLA-baked or Action animations are exported.
  The active Action on the armature object is exported if present;
  all stashed NLA tracks are exported as separate animations.
- Frame rate is read from the scene (default 60 fps assumed for NDS timing).
- Texture size is auto-detected from the first image texture node found.
"""

bl_info = {
    "name": "NDS Model Exporter",
    "author": "Taha Rashid",
    "version": (1, 0, 0),
    "blender": (3, 0, 0),
    "location": "File > Export > NDS Model (.zip)",
    "description": "Exports armature + mesh to NDS ZIP format (JSON + per-bone OBJs)",
    "category": "Import-Export",
}

import bpy
import bmesh
import mathutils
import os
import io
import json
import zipfile
import re
import struct

from bpy.props import StringProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def sanitize(name: str) -> str:
    return re.sub(r'[^a-zA-Z0-9]', '_', name)


def detect_texture_size(mesh_objects):
    """Walk material nodes looking for an Image Texture node."""
    for obj in mesh_objects:
        for mat in obj.data.materials:
            if mat is None:
                continue
            if mat.use_nodes:
                for node in mat.node_tree.nodes:
                    if node.type == 'TEX_IMAGE' and node.image:
                        img = node.image
                        return img.size[0], img.size[1]
    return None, None


def bone_index(armature, bone_name: str, bone_list: list) -> int:
    for i, b in enumerate(bone_list):
        if b.name == bone_name:
            return i
    return -1


def parent_index(bone, bone_list: list) -> int:
    if bone.parent is None:
        return -1
    return bone_index(None, bone.parent.name, bone_list)


# ---------------------------------------------------------------------------
# OBJ export per bone
# ---------------------------------------------------------------------------

def export_bone_obj(arm_obj, mesh_objects, bone, bone_list, depsgraph) -> str:
    """
    Build an OBJ string containing only the geometry weighted to `bone`.
    Works with vertex groups: group name must match bone name.
    """
    lines = ["# NDS OBJ export", f"# bone: {bone.name}", ""]

    vert_lines = []
    uv_lines   = []
    face_lines = []

    global_v_offset = 1   # OBJ indices are 1-based and global across meshes

    # We export in the armature's local space so origins make sense for the NDS
    arm_mat_inv = arm_obj.matrix_world.inverted()

    for mesh_obj in mesh_objects:
        if bone.name not in [vg.name for vg in mesh_obj.vertex_groups]:
            continue  # this mesh has no verts for this bone

        vg_index = mesh_obj.vertex_groups[bone.name].index

        # Evaluate the mesh (apply modifiers)
        eval_obj  = mesh_obj.evaluated_get(depsgraph)
        eval_mesh = eval_obj.to_mesh()

        # Triangulate a bmesh copy so we handle n-gons / quads cleanly
        bm = bmesh.new()
        bm.from_mesh(eval_mesh)
        # Don't triangulate — keep quads for NDS GL_QUADS; only triangulate n-gons
        # We'll handle faces below based on vert count.
        bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 4])
        bm.to_mesh(eval_mesh)
        bm.free()

        eval_mesh.calc_loop_triangles()
        uv_layer = eval_mesh.uv_layers.active

        # Build a map of vertex index -> weight for this bone's group
        bone_verts = {}
        for v in eval_mesh.vertices:
            for g in v.groups:
                if g.group == vg_index and g.weight > 0.5:
                    bone_verts[v.index] = True
                    break

        if not bone_verts:
            eval_obj.to_mesh_clear()
            continue

        # Collect unique positions
        local_verts = {}  # mesh vert index -> global OBJ vert index
        for vi, _w in bone_verts.items():
            world_co  = mesh_obj.matrix_world @ eval_mesh.vertices[vi].co
            local_co  = arm_mat_inv @ world_co
            vert_lines.append(f"v {local_co.x:.6f} {local_co.y:.6f} {local_co.z:.6f}")
            local_verts[vi] = global_v_offset
            global_v_offset += 1

        # Collect UVs per loop and faces
        uv_map = {}  # loop_index -> global UV index
        uv_counter = len(uv_lines) + 1

        for poly in eval_mesh.polygons:
            if not all(eval_mesh.loops[li].vertex_index in bone_verts
                       for li in poly.loop_indices):
                continue  # face not fully assigned to this bone — skip

            face_v  = []
            for li in poly.loop_indices:
                loop     = eval_mesh.loops[li]
                vi       = loop.vertex_index
                if uv_layer:
                    uv = uv_layer.data[li].uv
                    key = (round(uv.x, 5), round(uv.y, 5))
                    if key not in uv_map:
                        uv_lines.append(f"vt {uv.x:.6f} {uv.y:.6f}")
                        uv_map[key] = uv_counter
                        uv_counter += 1
                    uv_idx = uv_map[key]
                    face_v.append(f"{local_verts[vi]}/{uv_idx}")
                else:
                    face_v.append(f"{local_verts[vi]}")

            if len(face_v) >= 3:
                face_lines.append("f " + " ".join(face_v))

        eval_obj.to_mesh_clear()

    if not vert_lines:
        return "# Empty Node\n"

    lines += vert_lines
    lines += uv_lines
    lines += face_lines
    return "\n".join(lines) + "\n"


# ---------------------------------------------------------------------------
# Animation export
# ---------------------------------------------------------------------------

def get_bone_transforms_at_frame(arm_obj, bone_name, frame, scene):
    """
    Return (rot_xyz_deg, pos_xyz) of a pose bone at a given frame,
    relative to its rest pose / parent, in armature local space.
    """
    scene.frame_set(frame)
    pose_bone = arm_obj.pose.bones.get(bone_name)
    if pose_bone is None:
        return (0, 0, 0), (0, 0, 0)

    # Local matrix relative to parent (or armature root)
    if pose_bone.parent:
        mat = pose_bone.parent.matrix.inverted() @ pose_bone.matrix
    else:
        mat = pose_bone.matrix

    # Decompose
    loc, rot_q, _scale = mat.decompose()
    rot_e = rot_q.to_euler('XYZ')

    import math
    rx = math.degrees(rot_e.x)
    ry = math.degrees(rot_e.y)
    rz = math.degrees(rot_e.z)

    return (rx, ry, rz), (loc.x, loc.y, loc.z)


def collect_animations(arm_obj, bone_list, scene, fps):
    """
    Returns a dict compatible with the JSON format:
    {
      "anim_name": {
        "duration": <frames>,
        "tracks": {
          "<bone_id>": [ {time, rot:[x,y,z], pos:[x,y,z]}, ... ]
        }
      }
    }
    Exports the active Action if present, plus all NLA tracks (baked per strip).
    """
    animations = {}
    original_frame = scene.frame_current

    def export_action(action, anim_name):
        # Determine frame range
        frame_start = int(action.frame_range[0])
        frame_end   = int(action.frame_range[1])
        duration    = frame_end - frame_start + 1

        anim_data = {
            "duration": duration,
            "tracks": {}
        }

        # Gather keyframe times per bone from the action's fcurves
        bone_frames = {}
        for fcurve in action.fcurves:
            # data_path like 'pose.bones["Bone"].rotation_euler'
            if not fcurve.data_path.startswith('pose.bones'):
                continue
            # Extract bone name
            try:
                bname = fcurve.data_path.split('"')[1]
            except IndexError:
                continue
            if bname not in bone_frames:
                bone_frames[bname] = set()
            for kp in fcurve.keyframe_points:
                bone_frames[bname].add(int(round(kp.co[0])))

        for bone in bone_list:
            bname = bone.name
            if bname not in bone_frames:
                continue

            bid = bone_index(None, bname, bone_list)
            frames = sorted(bone_frames[bname])
            track  = []

            for f in frames:
                rel_frame = f - frame_start
                rot, pos  = get_bone_transforms_at_frame(arm_obj, bname, f, scene)
                track.append({
                    "time": rel_frame,
                    "rot":  list(rot),
                    "pos":  list(pos)
                })

            if track:
                anim_data["tracks"][str(bid)] = track

        animations[sanitize(anim_name)] = anim_data

    # Active action
    if arm_obj.animation_data and arm_obj.animation_data.action:
        action = arm_obj.animation_data.action
        export_action(action, action.name)

    # NLA strips
    if arm_obj.animation_data and arm_obj.animation_data.nla_tracks:
        for nla_track in arm_obj.animation_data.nla_tracks:
            for strip in nla_track.strips:
                if strip.action and sanitize(strip.action.name) not in animations:
                    export_action(strip.action, strip.action.name)

    scene.frame_set(original_frame)
    return animations


# ---------------------------------------------------------------------------
# Main export operator
# ---------------------------------------------------------------------------

class NDS_OT_ExportModel(bpy.types.Operator, ExportHelper):
    bl_idname  = "export_scene.nds_model"
    bl_label   = "Export NDS Model"
    bl_options = {'PRESET'}

    filename_ext = ".zip"
    filter_glob: StringProperty(default="*.zip", options={'HIDDEN'})

    def execute(self, context):
        scene      = context.scene
        depsgraph  = context.evaluated_depsgraph_get()

        # --- Find armature ---
        arm_obj = None
        for obj in scene.objects:
            if obj.type == 'ARMATURE' and obj.select_get():
                arm_obj = obj
                break
        if arm_obj is None:
            for obj in scene.objects:
                if obj.type == 'ARMATURE':
                    arm_obj = obj
                    break
        if arm_obj is None:
            self.report({'ERROR'}, "No armature found in scene.")
            return {'CANCELLED'}

        # --- Find mesh objects parented to armature ---
        mesh_objects = [
            o for o in scene.objects
            if o.type == 'MESH' and (
                o.parent == arm_obj or
                any(m.type == 'ARMATURE' and m.object == arm_obj
                    for m in o.modifiers)
            )
        ]
        if not mesh_objects:
            self.report({'ERROR'}, "No mesh objects found parented/bound to the armature.")
            return {'CANCELLED'}

        # --- Build bone list (ordered, flat) ---
        # Use edit bones for a stable order; pose bones have same names
        arm_obj.data.pose_position = 'REST'
        bone_list = list(arm_obj.data.bones)  # bpy.types.Bone, not edit bone

        # --- Detect texture ---
        tex_w, tex_h = detect_texture_size(mesh_objects)

        # --- Build model name ---
        base_name = sanitize(arm_obj.name or "model")
        tex_suffix = f"_{tex_w}x{tex_h}" if (tex_w and tex_h) else ""
        model_name = base_name + tex_suffix

        # --- Build ZIP ---
        zip_buffer = io.BytesIO()

        with zipfile.ZipFile(zip_buffer, 'w', zipfile.ZIP_DEFLATED) as zf:

            # 1. Per-bone OBJs
            nodes = []
            for i, bone in enumerate(bone_list):
                pid       = parent_index(bone, bone_list)
                obj_fname = f"{model_name}_n{i}.obj"

                # Export geometry for this bone
                arm_obj.data.pose_position = 'REST'
                obj_content = export_bone_obj(
                    arm_obj, mesh_objects, bone, bone_list, depsgraph
                )
                zf.writestr(obj_fname, obj_content)

                # Bone head in armature local space = origin
                origin = list(bone.head_local)  # [x, y, z]

                nodes.append({
                    "id":     i,
                    "parent": pid,
                    "name":   bone.name,
                    "obj":    obj_fname,
                    "origin": origin
                })

            # 2. Animations
            arm_obj.data.pose_position = 'POSE'
            fps   = scene.render.fps
            anims = collect_animations(arm_obj, bone_list, scene, fps)
            arm_obj.data.pose_position = 'REST'

            # 3. JSON
            ds_json = {"nodes": nodes, "animations": anims}
            zf.writestr(f"{model_name}.json", json.dumps(ds_json, indent=2))

        # Write ZIP to disk
        zip_buffer.seek(0)
        with open(self.filepath, 'wb') as f:
            f.write(zip_buffer.read())

        self.report({'INFO'}, f"Exported {model_name}.zip")
        return {'FINISHED'}


# ---------------------------------------------------------------------------
# Menu registration
# ---------------------------------------------------------------------------

def menu_func_export(self, context):
    self.layout.operator(NDS_OT_ExportModel.bl_idname, text="NDS Model (.zip)")


def register():
    bpy.utils.register_class(NDS_OT_ExportModel)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(NDS_OT_ExportModel)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()