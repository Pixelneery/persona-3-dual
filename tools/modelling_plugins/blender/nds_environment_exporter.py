"""
NDS Environment Exporter for Blender
Author: Taha Rashid / Persona 3 Dual project
Version: 1.0.0
Blender: 3.x / 4.x

Exports a scene or selected objects as a multi-texture OBJ + MTL,
then immediately runs obj2nds_environment.py to produce the .h file.

Location: File > Export > NDS Environment (.h)

HOW TO USE
──────────
1. Install this add-on (Edit > Preferences > Add-ons > Install).
2. Set up your scene:
   - Each material slot must have exactly ONE Image Texture node connected
     to the Base Color of the Principled BSDF (or any node with label/name
     containing "Base Color" or "Albedo").
   - Textures must be power-of-two (8, 16, 32, 64, 128, 256, 512, 1024).
   - All meshes must have UVs unwrapped.
3. Optionally apply all transforms: Ctrl+A > All Transforms.
4. File > Export > NDS Environment (.h).
5. Fill in the panel options, then click Export NDS Environment.
6. Two files are written to the output directory:
     modelName_env.h          — include this in your NDS project
     modelName_textures.txt   — run GRIT on every PNG listed here

PANEL OPTIONS
─────────────
• NDS Scale Mode:
    Auto  — scale so the longest object dimension = "Target Size" NDS units.
    Manual — multiply all vertex coords by "Scale Factor".
• Target Size   (Auto mode) : longest axis in NDS units. Default 4.0.
• Scale Factor  (Manual)    : explicit multiplier. Default 0.054.
• Centre Model  : translate so model is centred at X=0, Z=0, sits on Y=0.
• Selection Only: export only selected objects (default: all mesh objects).
• Converter Script: path to obj2nds_environment.py on your machine.
  If left blank the add-on just writes the OBJ+MTL and skips conversion.
"""

bl_info = {
    "name": "NDS Environment Exporter",
    "author": "Taha Rashid",
    "version": (1, 0, 0),
    "blender": (3, 0, 0),
    "location": "File > Export > NDS Environment (.h)",
    "description": "Exports scene as OBJ+MTL and converts to NDS display list header",
    "category": "Import-Export",
}

import bpy
import os
import re
import subprocess
import sys
import shutil

from bpy.props import (StringProperty, FloatProperty, BoolProperty,
                       EnumProperty)
from bpy_extras.io_utils import ExportHelper


VALID_TEX_SIZES = {8, 16, 32, 64, 128, 256, 512, 1024}


# ── Helpers ────────────────────────────────────────────────────────────────────

def sanitize(name):
    return re.sub(r'[^a-zA-Z0-9_]', '_', name)


def find_image_for_material(mat):
    """
    Return the first Image Texture node's image from the material node tree.
    Searches broadly — doesn't require a specific node connection order.
    """
    if mat is None or not mat.use_nodes:
        return None
    for node in mat.node_tree.nodes:
        if node.type == 'TEX_IMAGE' and node.image:
            return node.image
    return None


def get_mesh_objects(context, selection_only):
    if selection_only:
        return [o for o in context.selected_objects if o.type == 'MESH']
    return [o for o in context.scene.objects if o.type == 'MESH']


# ── Export logic ───────────────────────────────────────────────────────────────

def export_obj_mtl(mesh_objects, obj_path, depsgraph, apply_modifiers=True):
    """
    Write a plain OBJ + MTL from the given mesh objects.
    Each material slot becomes one `usemtl` group.
    Returns a dict: {material_name: png_abs_path}
    """
    obj_dir  = os.path.dirname(obj_path)
    mtl_name = os.path.splitext(os.path.basename(obj_path))[0] + '.mtl'
    mtl_path = os.path.join(obj_dir, mtl_name)

    v_lines   = []
    vt_lines  = []
    f_groups  = []   # list of (mat_name, face_lines)

    global_v  = 1    # 1-based OBJ indices
    global_vt = 1

    mat_to_png = {}  # sanitized_mat_name -> absolute png path
    written_mats = set()

    for obj in mesh_objects:
        eval_obj  = obj.evaluated_get(depsgraph) if apply_modifiers else obj
        eval_mesh = eval_obj.to_mesh()

        # Triangulate so we only emit tris (NDS friendly)
        import bmesh
        bm = bmesh.new()
        bm.from_mesh(eval_mesh)
        bmesh.ops.triangulate(bm, faces=bm.faces[:])
        bm.to_mesh(eval_mesh)
        bm.free()

        eval_mesh.calc_normals_split()
        uv_layer = eval_mesh.uv_layers.active

        # World-space transform
        mat_world = obj.matrix_world

        # Gather per-loop UVs
        loop_uvs = []
        if uv_layer:
            for loop in eval_mesh.loops:
                loop_uvs.append(uv_layer.data[loop.index].uv[:])
        else:
            loop_uvs = [(0.0, 0.0)] * len(eval_mesh.loops)

        # Map vertex index -> global OBJ index
        local_vi = {}
        for v in eval_mesh.vertices:
            co = mat_world @ v.co
            v_lines.append(f"v {co.x:.6f} {co.y:.6f} {co.z:.6f}")
            local_vi[v.index] = global_v
            global_v += 1

        # Map loop index -> global UV index (deduplicated per object)
        uv_key_to_gvt = {}
        local_vti = {}
        for loop in eval_mesh.loops:
            uv = loop_uvs[loop.index]
            key = (round(uv[0], 5), round(uv[1], 5))
            if key not in uv_key_to_gvt:
                vt_lines.append(f"vt {uv[0]:.6f} {uv[1]:.6f}")
                uv_key_to_gvt[key] = global_vt
                global_vt += 1
            local_vti[loop.index] = uv_key_to_gvt[key]

        # Group faces by material slot
        mat_faces = {}  # mat_index -> [face_strings]
        for poly in eval_mesh.polygons:
            mi  = poly.material_index
            mat = obj.material_slots[mi].material if mi < len(obj.material_slots) else None
            img = find_image_for_material(mat)

            # Derive a clean material name
            if mat:
                mat_id = sanitize(mat.name)
            else:
                mat_id = '__no_material__'

            # Register texture path
            if img and mat_id not in mat_to_png:
                # Try to get the file path
                try:
                    abs_path = bpy.path.abspath(img.filepath)
                    if not os.path.exists(abs_path) and img.packed_file:
                        # Packed image — save it next to the OBJ
                        png_name = sanitize(img.name)
                        if not png_name.endswith('.png'):
                            png_name += '.png'
                        abs_path = os.path.join(obj_dir, png_name)
                        img.save(filepath=abs_path)
                    mat_to_png[mat_id] = abs_path
                except Exception as e:
                    print(f"Warning: could not resolve texture for {mat_id}: {e}")

            face_str = "f"
            for li in poly.loop_indices:
                vi  = eval_mesh.loops[li].vertex_index
                vti = local_vti[li]
                face_str += f" {local_vi[vi]}/{vti}"

            if mat_id not in mat_faces:
                mat_faces[mat_id] = []
            mat_faces[mat_id].append(face_str)

        for mat_id, faces in mat_faces.items():
            f_groups.append((mat_id, faces))

        eval_obj.to_mesh_clear()

    # ── Write MTL ──────────────────────────────────────────────────────────────
    with open(mtl_path, 'w') as mf:
        mf.write(f"# NDS Environment Exporter\n")
        seen = set()
        for mat_id, _ in f_groups:
            if mat_id in seen:
                continue
            seen.add(mat_id)
            mf.write(f"\nnewmtl {mat_id}\n")
            mf.write(f"Ns 1.000000\nKa 1 1 1\nKd 1 1 1\nKs 0 0 0\n")
            if mat_id in mat_to_png:
                # Use just the filename — obj2nds_environment resolves relative to OBJ dir
                png_name = os.path.basename(mat_to_png[mat_id])
                # Copy texture to OBJ directory if not already there
                src = mat_to_png[mat_id]
                dst = os.path.join(obj_dir, png_name)
                if os.path.exists(src) and os.path.abspath(src) != os.path.abspath(dst):
                    shutil.copy2(src, dst)
                mf.write(f"map_Kd {png_name}\n")

    # ── Write OBJ ─────────────────────────────────────────────────────────────
    with open(obj_path, 'w') as of:
        of.write(f"# NDS Environment Exporter for Blender\n")
        of.write(f"mtllib {mtl_name}\n\n")
        of.write('\n'.join(v_lines) + '\n\n')
        of.write('\n'.join(vt_lines) + '\n\n')
        current_mat = None
        for mat_id, faces in f_groups:
            if mat_id != current_mat:
                of.write(f"\nusemtl {mat_id}\n")
                current_mat = mat_id
            of.write('\n'.join(faces) + '\n')

    return mat_to_png


# ── Operator ───────────────────────────────────────────────────────────────────

class NDS_OT_ExportEnvironment(bpy.types.Operator, ExportHelper):
    bl_idname  = "export_scene.nds_environment"
    bl_label   = "Export NDS Environment"
    bl_options = {'PRESET'}

    filename_ext = ".h"
    filter_glob: StringProperty(default="*.h", options={'HIDDEN'})

    # ── Panel properties ───────────────────────────────────────────────────────
    scale_mode: EnumProperty(
        name="NDS Scale Mode",
        items=[
            ('AUTO',   "Auto",   "Scale so longest axis = Target Size NDS units"),
            ('MANUAL', "Manual", "Use explicit Scale Factor multiplier"),
        ],
        default='AUTO',
    )
    target_size: FloatProperty(
        name="Target Size (NDS units)",
        description="Auto mode: longest axis of the model will be this many NDS units",
        default=4.0, min=0.01, max=8.0,
    )
    scale_factor: FloatProperty(
        name="Scale Factor",
        description="Manual mode: multiply all vertex coordinates by this value",
        default=0.054, min=0.0001, max=100.0,
    )
    centre_model: BoolProperty(
        name="Centre Model",
        description="Translate so model is centred on X/Z and sits on Y=0",
        default=True,
    )
    selection_only: BoolProperty(
        name="Selection Only",
        description="Export only selected mesh objects (default: all meshes in scene)",
        default=False,
    )
    converter_script: StringProperty(
        name="Converter Script",
        description="Path to obj2nds_environment.py. Leave blank to only write OBJ+MTL.",
        default="",
        subtype='FILE_PATH',
    )

    def draw(self, context):
        layout = self.layout
        layout.label(text="NDS Scale")
        layout.prop(self, "scale_mode")
        if self.scale_mode == 'AUTO':
            layout.prop(self, "target_size")
        else:
            layout.prop(self, "scale_factor")
        layout.separator()
        layout.prop(self, "centre_model")
        layout.prop(self, "selection_only")
        layout.separator()
        layout.label(text="Converter")
        layout.prop(self, "converter_script")

    def execute(self, context):
        depsgraph = context.evaluated_depsgraph_get()

        mesh_objects = get_mesh_objects(context, self.selection_only)
        if not mesh_objects:
            self.report({'ERROR'}, "No mesh objects found to export.")
            return {'CANCELLED'}

        # The ExportHelper filepath has .h extension; derive OBJ path alongside it
        h_path   = self.filepath
        out_dir  = os.path.dirname(h_path)
        base     = os.path.splitext(os.path.basename(h_path))[0]
        # Strip _env suffix if user typed it — we add it in the converter
        if base.endswith('_env'):
            base = base[:-4]
        obj_name = base + '.obj'
        obj_path = os.path.join(out_dir, obj_name)

        # ── Export OBJ + MTL ──────────────────────────────────────────────────
        self.report({'INFO'}, f"Exporting OBJ: {obj_path}")
        try:
            export_obj_mtl(mesh_objects, obj_path, depsgraph)
        except Exception as e:
            self.report({'ERROR'}, f"OBJ export failed: {e}")
            import traceback; traceback.print_exc()
            return {'CANCELLED'}

        # ── Run converter ─────────────────────────────────────────────────────
        script = self.converter_script.strip()
        if not script:
            self.report({'WARNING'},
                "No converter script set — OBJ+MTL written but not converted. "
                "Run obj2nds_environment.py manually.")
            return {'FINISHED'}

        script = bpy.path.abspath(script)
        if not os.path.exists(script):
            self.report({'ERROR'}, f"Converter script not found: {script}")
            return {'CANCELLED'}

        cmd = [sys.executable, script, obj_path, out_dir]
        if self.scale_mode == 'MANUAL':
            cmd += ['--scale', str(self.scale_factor)]
        else:
            cmd += ['--target-size', str(self.target_size)]
        if not self.centre_model:
            cmd += ['--no-center']

        self.report({'INFO'}, f"Running: {' '.join(cmd)}")
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
            if result.returncode != 0:
                self.report({'ERROR'},
                    f"Converter failed:\n{result.stderr[-500:]}")
                print("CONVERTER STDERR:", result.stderr)
                return {'CANCELLED'}
            print(result.stdout)
        except subprocess.TimeoutExpired:
            self.report({'ERROR'}, "Converter timed out.")
            return {'CANCELLED'}
        except Exception as e:
            self.report({'ERROR'}, f"Could not run converter: {e}")
            return {'CANCELLED'}

        self.report({'INFO'}, f"Done — {base}_env.h written to {out_dir}")
        return {'FINISHED'}


# ── Registration ───────────────────────────────────────────────────────────────

def menu_func_export(self, context):
    self.layout.operator(NDS_OT_ExportEnvironment.bl_idname,
                         text="NDS Environment (.h)")


def register():
    bpy.utils.register_class(NDS_OT_ExportEnvironment)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(NDS_OT_ExportEnvironment)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
