"""
NDS Model Exporter for Blender
Author: Taha Rashid
Version: 2.1.1 (Bake World Space Geometry)
Blender: 3.x / 4.x
"""

bl_info = {
    "name": "NDS Model Exporter",
    "author": "Taha Rashid",
    "version": (2, 1, 1),
    "blender": (3, 0, 0),
    "location": "File > Export > NDS Model (.zip)",
    "description": "Exports armature + mesh baked into Blender World Space",
    "category": "Import-Export",
}

import bpy, bmesh, io, json, os, re, tempfile, zipfile, math, mathutils
from collections import defaultdict
from bpy.props import StringProperty
from bpy_extras.io_utils import ExportHelper


def sanitize(name: str) -> str:
    return re.sub(r"[^a-zA-Z0-9]", "_", name)


def bone_index(armature, bone_name: str, bone_list: list) -> int:
    for i, b in enumerate(bone_list):
        if b.name == bone_name:
            return i
    return -1


def parent_index(bone, bone_list: list) -> int:
    if bone.parent is None:
        return -1
    return bone_index(None, bone.parent.name, bone_list)


def get_material_image(mat):
    if mat is None or not mat.use_nodes:
        return None
    for node in mat.node_tree.nodes:
        if node.type == "TEX_IMAGE" and node.image:
            return node.image
    return None


def tex_filename_for_image(image) -> str:
    return sanitize(os.path.splitext(image.name)[0]) + ".png"


def image_to_png_bytes(image) -> bytes:
    tmp_path = tempfile.mktemp(suffix=".png")
    try:
        orig_path, orig_format = image.filepath_raw, image.file_format
        image.filepath_raw, image.file_format = tmp_path, "PNG"
        image.save()
        image.filepath_raw, image.file_format = orig_path, orig_format
        with open(tmp_path, "rb") as f:
            return f.read()
    finally:
        if os.path.exists(tmp_path):
            os.unlink(tmp_path)


def build_mtl(mat_to_image: dict) -> str:
    lines = ["# NDS MTL export"]
    for mat_name, image in mat_to_image.items():
        lines.append(f"\nnewmtl {mat_name}")
        if image:
            lines.append(f"map_Kd {tex_filename_for_image(image)}")
    return "\n".join(lines) + "\n"


def export_bone_obj(arm_obj, mesh_objects, bone, depsgraph, obj_name: str):
    mtllib_name = obj_name + ".mtl"
    lines = ["# NDS OBJ export", f"# bone: {bone.name}", "", f"mtllib {mtllib_name}"]
    vert_lines, uv_lines, face_groups = [], [], []
    global_v_offset = 1
    mat_to_image = {}

    for mesh_obj in mesh_objects:
        if bone.name not in mesh_obj.vertex_groups:
            continue
        vg_index = mesh_obj.vertex_groups[bone.name].index
        eval_obj = mesh_obj.evaluated_get(depsgraph)
        eval_mesh = eval_obj.to_mesh()

        bm = bmesh.new()
        bm.from_mesh(eval_mesh)
        bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 4])
        bm.to_mesh(eval_mesh)
        bm.free()

        eval_mesh.calc_loop_triangles()
        uv_layer = eval_mesh.uv_layers.active
        mats = eval_mesh.materials

        bone_verts = set()
        faces_to_export = []
        for poly in eval_mesh.polygons:
            weights = defaultdict(float)
            for li in poly.loop_indices:
                v = eval_mesh.vertices[eval_mesh.loops[li].vertex_index]
                for g in v.groups:
                    weights[g.group] += g.weight
            if not weights:
                continue
            if max(weights, key=weights.get) == vg_index:
                faces_to_export.append(poly)
                for li in poly.loop_indices:
                    bone_verts.add(eval_mesh.loops[li].vertex_index)

        if not faces_to_export:
            eval_obj.to_mesh_clear()
            continue

        local_verts = {}
        for vi in sorted(bone_verts):
            # We export strictly to Blender World Space.
            # This bakes any global Armature object rotation/scale into the geometry
            # so the model looks upright based on Blender's global axes.
            world_co = mesh_obj.matrix_world @ eval_mesh.vertices[vi].co
            vert_lines.append(f"v {world_co.x:.6f} {world_co.y:.6f} {world_co.z:.6f}")
            local_verts[vi] = global_v_offset
            global_v_offset += 1

        uv_map, uv_counter = {}, len(uv_lines) + 1
        mat_face_map = defaultdict(list)

        for poly in faces_to_export:
            mat = mats[poly.material_index] if poly.material_index < len(mats) else None
            mat_name = sanitize(mat.name) if mat else "__no_material__"
            if mat_name not in mat_to_image:
                mat_to_image[mat_name] = get_material_image(mat)

            face_v = []
            for li in poly.loop_indices:
                vi = eval_mesh.loops[li].vertex_index
                if uv_layer:
                    uv = uv_layer.data[li].uv
                    key = (round(uv.x, 5), round(uv.y, 5))
                    if key not in uv_map:
                        uv_lines.append(f"vt {uv.x:.6f} {uv.y:.6f}")
                        uv_map[key] = uv_counter
                        uv_counter += 1
                    face_v.append(f"{local_verts[vi]}/{uv_map[key]}")
                else:
                    face_v.append(f"{local_verts[vi]}")
            if len(face_v) >= 3:
                mat_face_map[mat_name].append("f " + " ".join(face_v))

        for mat_name, faces in mat_face_map.items():
            face_groups.append((mat_name, faces))
        eval_obj.to_mesh_clear()

    if not vert_lines:
        return "# Empty Node\n", {}
    lines += vert_lines + uv_lines
    for mat_name, faces in face_groups:
        lines.append(f"usemtl {mat_name}")
        lines.extend(faces)
    return "\n".join(lines) + "\n", mat_to_image


def get_bone_transforms_at_frame(arm_obj, bone_name, frame, scene):
    scene.frame_set(frame)
    pose_bone = arm_obj.pose.bones.get(bone_name)
    bone = arm_obj.data.bones.get(bone_name)

    if pose_bone is None or bone is None:
        return (0, 0, 0), (0, 0, 0)

    # World Space Animation Math.
    # To support global Armature rotation, we must calculate the delta relative
    # to the TRUE World, not armature-local space.
    mw = arm_obj.matrix_world

    # Absolute deformation (Rest -> Pose) map in true World space
    world_pose_n = mw @ pose_bone.matrix
    world_rest_n = mw @ bone.matrix_local
    delta_n = world_pose_n @ world_rest_n.inverted()

    if bone.parent:
        pose_parent = arm_obj.pose.bones[bone.parent.name]
        bone_parent = arm_obj.data.bones[bone.parent.name]
        world_pose_p = mw @ pose_parent.matrix
        world_rest_p = mw @ bone_parent.matrix_local
        delta_p = world_pose_p @ world_rest_p.inverted()
    else:
        delta_p = mathutils.Matrix.Identity(4)

    # Relative world delta (Animation transform in world-aligned space)
    rel = delta_p.inverted() @ delta_n

    # The NDS engine rotates around the node's origin. We find the local matrix
    # that applied at the origin produces the calculated 'rel' deformation.
    # Origin must also be true World space.
    origin_n_w = mw @ bone.head_local
    T_orig = mathutils.Matrix.Translation(origin_n_w)
    T_orig_inv = mathutils.Matrix.Translation(-origin_n_w)

    # M = T_inv @ Rel @ T
    M = T_orig_inv @ rel @ T_orig

    loc, rot_q, _scale = M.decompose()
    rot_e = rot_q.to_euler("XYZ")

    return (math.degrees(rot_e.x), math.degrees(rot_e.y), math.degrees(rot_e.z)), (
        loc.x,
        loc.y,
        loc.z,
    )


def collect_animations(arm_obj, bone_list, scene):
    animations, original_frame = {}, scene.frame_current

    def export_action(action, anim_name):
        f_start, f_end = int(action.frame_range[0]), int(action.frame_range[1])
        anim_data = {"duration": f_end - f_start + 1, "tracks": {}}
        bone_frames = defaultdict(set)

        for fcurve in action.fcurves:
            if not fcurve.data_path.startswith("pose.bones"):
                continue
            try:
                bname = fcurve.data_path.split('"')[1]
            except IndexError:
                continue
            for kp in fcurve.keyframe_points:
                bone_frames[bname].add(int(round(kp.co[0])))

        for bone in bone_list:
            if bone.name not in bone_frames:
                continue
            bid = bone_index(None, bone.name, bone_list)
            track = []
            for f in sorted(bone_frames[bone.name]):
                rot, pos = get_bone_transforms_at_frame(arm_obj, bone.name, f, scene)
                track.append({"time": f - f_start, "rot": list(rot), "pos": list(pos)})
            if track:
                anim_data["tracks"][str(bid)] = track

        animations[sanitize(anim_name)] = anim_data

    # Ensure animation data exists
    if not arm_obj.animation_data:
        arm_obj.animation_data_create()

    # Remember the originally active action so we can put it back later
    orig_action = arm_obj.animation_data.action

    # Loop through ALL actions saved in the Blender file
    for action in bpy.data.actions:
        # Temporarily apply the action to the armature so scene.frame_set reads the correct poses
        arm_obj.animation_data.action = action
        export_action(action, action.name)

    # Restore the original action
    arm_obj.animation_data.action = orig_action

    scene.frame_set(original_frame)
    return animations


class NDS_OT_ExportModel(bpy.types.Operator, ExportHelper):
    bl_idname, bl_label, bl_options = (
        "export_scene.nds_model",
        "Export NDS Model",
        {"PRESET"},
    )
    filename_ext = ".zip"
    filter_glob: StringProperty(default="*.zip", options={"HIDDEN"})

    def execute(self, context):
        scene = context.scene
        arm_obj = next(
            (o for o in scene.objects if o.type == "ARMATURE" and o.select_get()), None
        )
        if not arm_obj:
            arm_obj = next((o for o in scene.objects if o.type == "ARMATURE"), None)
        if not arm_obj:
            self.report({"ERROR"}, "No armature found.")
            return {"CANCELLED"}

        mesh_objects = [
            o
            for o in scene.objects
            if o.type == "MESH"
            and (
                o.parent == arm_obj
                or any(
                    m.type == "ARMATURE" and m.object == arm_obj for m in o.modifiers
                )
            )
        ]

        model_name = sanitize(os.path.splitext(os.path.basename(self.filepath))[0])
        arm_obj.data.pose_position = "REST"
        context.view_layer.update()
        depsgraph = context.evaluated_depsgraph_get()
        bone_list = list(arm_obj.data.bones)

        zip_buffer = io.BytesIO()
        with zipfile.ZipFile(zip_buffer, "w", zipfile.ZIP_DEFLATED) as zf:
            nodes, all_tex_images = [], {}

            for i, bone in enumerate(bone_list):
                obj_name = f"{model_name}_n{i}"
                obj_content, mat_to_image = export_bone_obj(
                    arm_obj, mesh_objects, bone, depsgraph, obj_name
                )
                zf.writestr(f"{obj_name}.obj", obj_content)
                zf.writestr(f"{obj_name}.mtl", build_mtl(mat_to_image))

                for img in mat_to_image.values():
                    if img:
                        all_tex_images[tex_filename_for_image(img)] = img

                # Export True Blender World Space Origin
                # This ensures pivot points are correctly placed if Armature is globals rotated.
                world_head = arm_obj.matrix_world @ bone.head_local
                nodes.append(
                    {
                        "id": i,
                        "parent": parent_index(bone, bone_list),
                        "name": bone.name,
                        "obj": f"{obj_name}.obj",
                        "origin": list(world_head),
                    }
                )

            for tex_fn, image in all_tex_images.items():
                try:
                    zf.writestr(tex_fn, image_to_png_bytes(image))
                except Exception as e:
                    self.report({"WARNING"}, f"Texture error: {e}")

            arm_obj.data.pose_position = "POSE"
            anims = collect_animations(arm_obj, bone_list, scene)
            arm_obj.data.pose_position = "REST"

            zf.writestr(
                f"{model_name}.json",
                json.dumps({"nodes": nodes, "animations": anims}, indent=2),
            )

        zip_buffer.seek(0)
        with open(self.filepath, "wb") as f:
            f.write(zip_buffer.read())
        return {"FINISHED"}


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
