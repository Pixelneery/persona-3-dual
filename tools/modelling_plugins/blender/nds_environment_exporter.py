"""
NDS Environment Exporter for Blender
Author: Taha Rashid / Persona 3 Dual project

Exports a scene to NDS display list.
"""

bl_info = {
    "name": "NDS Environment Exporter",
    "author": "Taha Rashid",
    "version": (2, 2, 1),
    "blender": (3, 0, 0),
    "location": "File > Export > NDS Environment (.h)",
    "description": "Exports scene to NDS display list.",
    "category": "Import-Export",
}

import bpy, os, re, subprocess, shutil
from bpy.props import (StringProperty, FloatProperty, BoolProperty, EnumProperty)
from bpy_extras.io_utils import ExportHelper

# ── Helpers ────────────────────────────────────────────────────────────────────

def sanitize(name):
    return re.sub(r'[^a-zA-Z0-9_]', '_', name)

def find_image_for_material(mat):
    if mat is None or not mat.use_nodes: return None
    for node in mat.node_tree.nodes:
        if node.type == 'TEX_IMAGE' and node.image:
            return node.image
    return None

def get_mesh_objects(context, selection_only):
    src = context.selected_objects if selection_only else context.scene.objects
    return [o for o in src if o.type == 'MESH']

# ── Preferences ────────────────────────────────────────────────────────────────

class NDS_Environment_Preferences(bpy.types.AddonPreferences):
    bl_idname = __name__

    converter_script: StringProperty(
        name="Build Script",
        description="Absolute path to nds_build_environment.py",
        default="", subtype='FILE_PATH')
    mapping_file: StringProperty(
        name="Material Mapping JSON",
        description="Optional: maps material names to PNG paths",
        default="", subtype='FILE_PATH')

    def draw(self, context):
        layout = self.layout
        box = layout.box()
        box.label(text="Paths", icon='FILE_FOLDER')
        box.prop(self, "converter_script")
        box.prop(self, "mapping_file")
        if not self.converter_script:
            layout.label(text="Set build script path to enable export!", icon='ERROR')

# ── OBJ export ─────────────────────────────────────────────────────────────────

def export_obj_mtl(mesh_objects, obj_path, depsgraph):
    import bmesh
    obj_dir  = os.path.dirname(obj_path)
    mtl_name = os.path.splitext(os.path.basename(obj_path))[0] + '.mtl'
    mtl_path = os.path.join(obj_dir, mtl_name)
    v_lines, vt_lines, f_groups = [], [], []
    global_v = global_vt = 1
    mat_to_png = {}

    for obj in mesh_objects:
        eval_obj  = obj.evaluated_get(depsgraph)
        eval_mesh = eval_obj.to_mesh()
        bm = bmesh.new(); bm.from_mesh(eval_mesh)
        bmesh.ops.triangulate(bm, faces=bm.faces[:]); bm.to_mesh(eval_mesh); bm.free()
        eval_mesh.calc_normals_split()
        uv_layer  = eval_mesh.uv_layers.active
        mat_world = obj.matrix_world
        loop_uvs  = ([uv_layer.data[l.index].uv[:] for l in eval_mesh.loops]
                     if uv_layer else [(0.0, 0.0)] * len(eval_mesh.loops))

        local_vi = {}
        for v in eval_mesh.vertices:
            co = mat_world @ v.co
            v_lines.append(f"v {co.x:.6f} {co.y:.6f} {co.z:.6f}")
            local_vi[v.index] = global_v; global_v += 1

        uv_key_map, local_vti = {}, {}
        for loop in eval_mesh.loops:
            uv  = loop_uvs[loop.index]
            key = (round(uv[0], 5), round(uv[1], 5))
            if key not in uv_key_map:
                vt_lines.append(f"vt {uv[0]:.6f} {uv[1]:.6f}")
                uv_key_map[key] = global_vt; global_vt += 1
            local_vti[loop.index] = uv_key_map[key]

        mat_faces = {}
        for poly in eval_mesh.polygons:
            mi  = poly.material_index
            mat = obj.material_slots[mi].material if mi < len(obj.material_slots) else None
            img = find_image_for_material(mat)
            mat_id = sanitize(mat.name) if mat else '__no_material__'
            if img and mat_id not in mat_to_png:
                try:
                    abs_path = bpy.path.abspath(img.filepath)
                    if not os.path.exists(abs_path) and img.packed_file:
                        png_name = sanitize(img.name)
                        if not png_name.endswith('.png'): png_name += '.png'
                        abs_path = os.path.join(obj_dir, png_name)
                        img.save(filepath=abs_path)
                    mat_to_png[mat_id] = abs_path
                except Exception: pass
            face_str = "f" + "".join(
                f" {local_vi[eval_mesh.loops[li].vertex_index]}/{local_vti[li]}"
                for li in poly.loop_indices)
            mat_faces.setdefault(mat_id, []).append(face_str)

        for mat_id, faces in mat_faces.items():
            f_groups.append((mat_id, faces))
        eval_obj.to_mesh_clear()

    with open(mtl_path, 'w') as mf:
        mf.write("# NDS Environment Exporter\n")
        seen = set()
        for mat_id, _ in f_groups:
            if mat_id in seen: continue
            seen.add(mat_id)
            mf.write(f"\nnewmtl {mat_id}\nNs 1\nKa 1 1 1\nKd 1 1 1\nKs 0 0 0\n")
            if mat_id in mat_to_png:
                png_name = os.path.basename(mat_to_png[mat_id])
                src = mat_to_png[mat_id]; dst = os.path.join(obj_dir, png_name)
                if os.path.exists(src) and os.path.abspath(src) != os.path.abspath(dst):
                    shutil.copy2(src, dst)
                mf.write(f"map_Kd {png_name}\n")

    with open(obj_path, 'w') as of:
        of.write(f"# NDS Environment Exporter\nmtllib {mtl_name}\n\n")
        of.write('\n'.join(v_lines) + '\n\n')
        of.write('\n'.join(vt_lines) + '\n\n')
        current_mat = None
        for mat_id, faces in f_groups:
            if mat_id != current_mat:
                of.write(f"\nusemtl {mat_id}\n"); current_mat = mat_id
            of.write('\n'.join(faces) + '\n')

    return mat_to_png

# ── Export operator ─────────────────────────────────────────────────────────────

class NDS_OT_ExportEnvironment(bpy.types.Operator, ExportHelper):
    bl_idname  = "export_scene.nds_environment"
    bl_label   = "Export NDS Environment"
    bl_options = {'PRESET'}

    filename_ext  = ".h"
    filter_glob:   StringProperty(default="*.h", options={'HIDDEN'})
    scale_mode:    EnumProperty(name="Scale Mode",
                                items=[('AUTO', "Auto Size", ""),
                                       ('MANUAL', "Manual Scale", "")],
                                default='AUTO')
    target_size:   FloatProperty(name="Target Size",  default=4.0,   min=0.01)
    scale_factor:  FloatProperty(name="Scale Factor", default=0.054, min=0.0001)
    centre_model:  BoolProperty(name="Centre Model",  default=True)
    selection_only:BoolProperty(name="Selection Only",default=False)
    skip_grit:     BoolProperty(name="Skip GRIT",     default=False)
    grit_flags:    StringProperty(name="GRIT Flags",  default="-fts -fh -gb -gB16 -pu16")

    def draw(self, context):
        layout = self.layout

        box = layout.box(); box.label(text="Geometry", icon='MESH_DATA')
        box.prop(self, "scale_mode")
        box.prop(self, "target_size" if self.scale_mode == 'AUTO' else "scale_factor")
        box.prop(self, "centre_model"); box.prop(self, "selection_only")

        box = layout.box(); box.label(text="Textures / GRIT", icon='TEXTURE')
        box.prop(self, "skip_grit")
        if not self.skip_grit: box.prop(self, "grit_flags")

    def execute(self, context):
        prefs  = context.preferences.addons[__name__].preferences
        script = bpy.path.abspath(prefs.converter_script.strip())

        if not os.path.exists(script):
            self.report({'ERROR'}, "Build script not found — check Preferences.")
            return {'CANCELLED'}

        mesh_objects = get_mesh_objects(context, self.selection_only)
        if not mesh_objects:
            self.report({'ERROR'}, "No mesh objects found.")
            return {'CANCELLED'}

        h_path   = self.filepath
        out_dir  = os.path.dirname(h_path)
        base     = os.path.splitext(os.path.basename(h_path))[0]
        if base.endswith('_env'): base = base[:-4]
        obj_path = os.path.join(out_dir, base + '.obj')

        try:
            export_obj_mtl(mesh_objects, obj_path, context.evaluated_depsgraph_get())
        except Exception as e:
            self.report({'ERROR'}, f"OBJ export failed: {e}")
            return {'CANCELLED'}

        py  = shutil.which("python3") or shutil.which("python") or "python"
        cmd = [py, script, obj_path, out_dir]

        if self.scale_mode == 'MANUAL': cmd += ['--scale',       str(self.scale_factor)]
        else:                           cmd += ['--target-size',  str(self.target_size)]
        if not self.centre_model: cmd += ['--no-center']
        if self.skip_grit:        cmd += ['--skip-grit']
        elif self.grit_flags:     cmd += ['--grit-flags', self.grit_flags]
        if prefs.mapping_file:    cmd += ['--mapping', bpy.path.abspath(prefs.mapping_file)]
        cmd += ['--source-blender']

        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=180)
            if result.returncode != 0:
                self.report({'ERROR'}, f"Pipeline failed:\n{result.stderr[-600:]}")
                print("STDERR:", result.stderr)
                return {'CANCELLED'}
            print(result.stdout)
        except subprocess.TimeoutExpired:
            self.report({'ERROR'}, "Pipeline timed out (180s).")
            return {'CANCELLED'}
        except Exception as e:
            self.report({'ERROR'}, f"Could not run pipeline: {e}")
            return {'CANCELLED'}

        self.report({'INFO'}, f"Done — {out_dir}")
        return {'FINISHED'}

# ── Registration ───────────────────────────────────────────────────────────────

def menu_func_export(self, context):
    self.layout.operator(NDS_OT_ExportEnvironment.bl_idname,
                         text="NDS Environment (.h)")

classes = (
    NDS_Environment_Preferences,
    NDS_OT_ExportEnvironment,
)

def register():
    for cls in classes: bpy.utils.register_class(cls)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
    for cls in reversed(classes): bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()