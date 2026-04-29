"""
NDS Environment Exporter for Blender
Author: Taha Rashid / Persona 3 Dual project
Version: 1.9.0
Blender: 3.x / 4.x

Exports a scene to NDS display list.
Features a live JSON palette for texture painting and manual tilemap macro injection.
"""

bl_info = {
    "name": "NDS Environment Exporter",
    "author": "Taha Rashid",
    "version": (1, 9, 0),
    "blender": (3, 0, 0),
    "location": "File > Export > NDS Environment (.h) | Sidebar (N) > NDS",
    "description": "Exports scene to NDS display list with a built-in JSON collision palette editor.",
    "category": "Import-Export",
}

import bpy
import os
import re
import subprocess
import sys
import shutil
import json

from bpy.props import (StringProperty, FloatProperty, BoolProperty,
                       EnumProperty, IntProperty, CollectionProperty, FloatVectorProperty)
from bpy_extras.io_utils import ExportHelper

VALID_TEX_SIZES = {8, 16, 32, 64, 128, 256, 512, 1024}

# ── Helpers ────────────────────────────────────────────────────────────────────

def sanitize(name):
    return re.sub(r'[^a-zA-Z0-9_]', '_', name)

def find_image_for_material(mat):
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


# ── Built-in Palette Editor & Sync Logic ───────────────────────────────────────

class NDSCollisionColor(bpy.types.PropertyGroup):
    name: StringProperty(name="Name", default="New State")
    color: FloatVectorProperty(name="Color", subtype='COLOR', default=(1.0, 0.0, 0.0), min=0.0, max=1.0)
    state_id: IntProperty(name="ID", default=1, min=1)

class NDS_UL_PaletteList(bpy.types.UIList):
    def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):
        row = layout.row(align=True)
        
        is_painting = hasattr(context.tool_settings, "image_paint") and context.tool_settings.image_paint.brush is not None
        op_row = row.row(align=True)
        op_row.enabled = is_painting
        op = op_row.operator("nds.set_brush_color", text="", icon='BRUSH_DATA')
        op.color = item.color
        
        row.prop(item, "color", text="")
        row.prop(item, "name", text="", emboss=False)
        row.prop(item, "state_id", text="ID")

def load_palette_from_disk(prefs):
    path = bpy.path.abspath(prefs.palette_file)
    if not path or not os.path.exists(path): return False
    try:
        with open(path, 'r') as f:
            data = json.load(f)
        prefs.collision_palette.clear()
        for name, info in data.items():
            item = prefs.collision_palette.add()
            item.name = name
            item.state_id = info['id']
            item.color = (info['rgb'][0]/255.0, info['rgb'][1]/255.0, info['rgb'][2]/255.0)
        return True
    except: return False

def save_palette_to_disk(prefs):
    path = bpy.path.abspath(prefs.palette_file)
    if not path: return False
    data = {}
    for item in prefs.collision_palette:
        r = int(item.color[0] * 255)
        g = int(item.color[1] * 255)
        b = int(item.color[2] * 255)
        data[item.name] = {"rgb": [r, g, b], "id": item.state_id}
    try:
        with open(path, 'w') as f:
            json.dump(data, f, indent=4)
        return True
    except: return False

class NDS_OT_LoadPalette(bpy.types.Operator):
    bl_idname = "nds.load_palette"
    bl_label = "Load / Sync"
    bl_description = "Pulls colors from the shared JSON file into Blender (Overwrites current unsaved edits)"
    def execute(self, context):
        prefs = context.preferences.addons[__name__].preferences
        if load_palette_from_disk(prefs): self.report({'INFO'}, "Palette Loaded")
        else: self.report({'ERROR'}, "Could not load JSON file")
        return {'FINISHED'}

class NDS_OT_SavePalette(bpy.types.Operator):
    bl_idname = "nds.save_palette"
    bl_label = "Save JSON"
    bl_description = "Pushes Blender's colors to your team's shared JSON file on disk"
    def execute(self, context):
        prefs = context.preferences.addons[__name__].preferences
        if save_palette_to_disk(prefs): self.report({'INFO'}, "Palette Saved")
        else: self.report({'ERROR'}, "Could not save JSON file")
        return {'FINISHED'}

class NDS_OT_AddPaletteItem(bpy.types.Operator):
    bl_idname = "nds.add_palette_item"
    bl_label = "Add State"
    def execute(self, context):
        prefs = context.preferences.addons[__name__].preferences
        prefs.collision_palette.add()
        prefs.palette_index = len(prefs.collision_palette) - 1
        return {'FINISHED'}

class NDS_OT_RemovePaletteItem(bpy.types.Operator):
    bl_idname = "nds.remove_palette_item"
    bl_label = "Remove State"
    def execute(self, context):
        prefs = context.preferences.addons[__name__].preferences
        if len(prefs.collision_palette) > 0:
            prefs.collision_palette.remove(prefs.palette_index)
            prefs.palette_index = min(max(0, prefs.palette_index - 1), len(prefs.collision_palette) - 1)
        return {'FINISHED'}

class NDS_OT_SetBrushColor(bpy.types.Operator):
    bl_idname = "nds.set_brush_color"
    bl_label = "Set Brush Color"
    bl_description = "Instantly sets your active paint brush to this exact color"
    
    color: FloatVectorProperty(size=3, subtype='COLOR')

    def execute(self, context):
        context.tool_settings.image_paint.brush.color = self.color
        return {'FINISHED'}


# ── Add-on Preferences ─────────────────────────────────────────────────────────

class NDS_Environment_Preferences(bpy.types.AddonPreferences):
    bl_idname = __name__

    converter_script: StringProperty(
        name="Converter Script",
        description="Absolute path to your nds_build_environment.py wrapper script",
        default="", subtype='FILE_PATH',
    )
    
    mapping_file: StringProperty(
        name="Material JSON Mapping",
        description="Optional: Path to a JSON explicitly mapping material names to PNGs",
        default="", subtype='FILE_PATH',
    )

    palette_file: StringProperty(
        name="Shared Palette JSON",
        description="Absolute path to your team's shared collision_palette.json",
        default="", subtype='FILE_PATH',
    )

    collision_palette: CollectionProperty(type=NDSCollisionColor)
    palette_index: IntProperty(default=0)

    def draw(self, context):
        layout = self.layout
        
        box = layout.box()
        box.label(text="Paths & Scripts", icon='FILE_FOLDER')
        box.prop(self, "converter_script")
        box.prop(self, "mapping_file")
        
        box = layout.box()
        box.label(text="Collision Setup", icon='SNAP_GRID')
        box.prop(self, "palette_file")
        
        if not self.converter_script:
            layout.label(text="Please select the path to the Python script to enable auto-conversion!", icon='ERROR')


# ── Palette Sidebar Panel ──────────────────────────────────────────────────────

def draw_palette_sidebar(self, context):
    layout = self.layout
    prefs = context.preferences.addons[__name__].preferences
    
    if not prefs.palette_file or not os.path.exists(bpy.path.abspath(prefs.palette_file)):
        layout.label(text="Palette JSON not found.", icon='ERROR')
        layout.label(text="Set the path in Edit > Preferences")
        return
        
    row = layout.row(align=True)
    row.operator("nds.load_palette", icon='FILE_REFRESH')
    row.operator("nds.save_palette", icon='FILE_TICK')
    
    layout.separator()
    layout.label(text="Editable Collision States:", icon='COLOR')
    
    layout.template_list("NDS_UL_PaletteList", "", prefs, "collision_palette", prefs, "palette_index")
    
    row = layout.row(align=True)
    row.operator("nds.add_palette_item", icon='ADD')
    row.operator("nds.remove_palette_item", icon='REMOVE')
    
    layout.separator()
    layout.label(text="Tip: Click the brush icon to paint!", icon='INFO')

class NDS_PT_PaletteLegend_Image(bpy.types.Panel):
    bl_space_type = 'IMAGE_EDITOR'
    bl_region_type = 'UI'
    bl_category = 'NDS'
    bl_label = "Collision Palette Editor"
    def draw(self, context): draw_palette_sidebar(self, context)

class NDS_PT_PaletteLegend_3D(bpy.types.Panel):
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'NDS'
    bl_label = "Collision Palette Editor"
    def draw(self, context): draw_palette_sidebar(self, context)


# ── Export logic ───────────────────────────────────────────────────────────────

def export_obj_mtl(mesh_objects, obj_path, depsgraph, apply_modifiers=True):
    obj_dir  = os.path.dirname(obj_path)
    mtl_name = os.path.splitext(os.path.basename(obj_path))[0] + '.mtl'
    mtl_path = os.path.join(obj_dir, mtl_name)

    v_lines   = []
    vt_lines  = []
    f_groups  = []   

    global_v  = 1    
    global_vt = 1

    mat_to_png = {}  

    for obj in mesh_objects:
        eval_obj  = obj.evaluated_get(depsgraph) if apply_modifiers else obj
        eval_mesh = eval_obj.to_mesh()

        import bmesh
        bm = bmesh.new()
        bm.from_mesh(eval_mesh)
        bmesh.ops.triangulate(bm, faces=bm.faces[:])
        bm.to_mesh(eval_mesh)
        bm.free()

        eval_mesh.calc_normals_split()
        uv_layer = eval_mesh.uv_layers.active
        mat_world = obj.matrix_world

        loop_uvs = []
        if uv_layer:
            for loop in eval_mesh.loops:
                loop_uvs.append(uv_layer.data[loop.index].uv[:])
        else:
            loop_uvs = [(0.0, 0.0)] * len(eval_mesh.loops)

        local_vi = {}
        for v in eval_mesh.vertices:
            co = mat_world @ v.co
            v_lines.append(f"v {co.x:.6f} {co.y:.6f} {co.z:.6f}")
            local_vi[v.index] = global_v
            global_v += 1

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
                except: pass

            face_str = "f"
            for li in poly.loop_indices:
                vi  = eval_mesh.loops[li].vertex_index
                vti = local_vti[li]
                face_str += f" {local_vi[vi]}/{vti}"

            if mat_id not in mat_faces: mat_faces[mat_id] = []
            mat_faces[mat_id].append(face_str)

        for mat_id, faces in mat_faces.items():
            f_groups.append((mat_id, faces))

        eval_obj.to_mesh_clear()

    with open(mtl_path, 'w') as mf:
        mf.write(f"# NDS Environment Exporter\n")
        seen = set()
        for mat_id, _ in f_groups:
            if mat_id in seen: continue
            seen.add(mat_id)
            mf.write(f"\nnewmtl {mat_id}\nNs 1.000000\nKa 1 1 1\nKd 1 1 1\nKs 0 0 0\n")
            if mat_id in mat_to_png:
                png_name = os.path.basename(mat_to_png[mat_id])
                src = mat_to_png[mat_id]
                dst = os.path.join(obj_dir, png_name)
                if os.path.exists(src) and os.path.abspath(src) != os.path.abspath(dst):
                    shutil.copy2(src, dst)
                mf.write(f"map_Kd {png_name}\n")

    with open(obj_path, 'w') as of:
        of.write(f"# NDS Environment Exporter for Blender\nmtllib {mtl_name}\n\n")
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

    # ── Geometry properties ──
    scale_mode: EnumProperty(
        name="Scale Mode",
        items=[('AUTO', "Auto Size", ""), ('MANUAL', "Manual Scale", "")], default='AUTO'
    )
    target_size: FloatProperty(name="Target Size", default=4.0, min=0.01)
    scale_factor: FloatProperty(name="Scale Factor", default=0.054, min=0.0001)
    centre_model: BoolProperty(name="Centre Model on Origin", default=True)
    selection_only: BoolProperty(name="Export Selection Only", default=False)
    
    # ── Textures properties ──
    skip_grit: BoolProperty(name="Skip GRIT Conversion", default=False)
    grit_flags: StringProperty(name="GRIT Flags", default="-ftc -fh -gb -gB16 -pu16")

    # ── Collision properties ──
    inject_tiles: BoolProperty(name="Inject Tilemap Macros", default=False)
    tile_start_x: FloatProperty(name="Start X Offset", default=0.0)
    tile_start_z: FloatProperty(name="Start Z Offset", default=0.0)
    tile_cols: IntProperty(name="Map Columns", default=64, min=1)
    tile_rows: IntProperty(name="Map Rows", default=64, min=1)

    def draw(self, context):
        layout = self.layout
        
        box = layout.box()
        box.label(text="Geometry & Scale", icon='MESH_DATA')
        box.prop(self, "scale_mode")
        if self.scale_mode == 'AUTO': box.prop(self, "target_size")
        else: box.prop(self, "scale_factor")
        box.prop(self, "centre_model")
        box.prop(self, "selection_only")
        
        box = layout.box()
        box.label(text="Textures & GRIT", icon='TEXTURE')
        box.prop(self, "skip_grit")
        if not self.skip_grit: box.prop(self, "grit_flags")
        
        box = layout.box()
        box.label(text="Collision Macros", icon='SNAP_GRID')
        box.prop(self, "inject_tiles")
        
        if self.inject_tiles:
            inner = box.box()
            row_ui = inner.row()
            row_ui.prop(self, "tile_start_x", text="X Offset")
            row_ui.prop(self, "tile_start_z", text="Z Offset")
            row_ui = inner.row()
            row_ui.prop(self, "tile_cols", text="Cols")
            row_ui.prop(self, "tile_rows", text="Rows")

    def execute(self, context):
        depsgraph = context.evaluated_depsgraph_get()
        mesh_objects = get_mesh_objects(context, self.selection_only)
        if not mesh_objects:
            self.report({'ERROR'}, "No mesh objects found.")
            return {'CANCELLED'}

        h_path   = self.filepath
        out_dir  = os.path.dirname(h_path)
        base     = os.path.splitext(os.path.basename(h_path))[0]
        if base.endswith('_env'): base = base[:-4]
        obj_path = os.path.join(out_dir, base + '.obj')

        # Find the system python
        python_bin = shutil.which("python3") or shutil.which("python") or "python"

        # 1. Export OBJ
        try:
            export_obj_mtl(mesh_objects, obj_path, depsgraph)
        except Exception as e:
            self.report({'ERROR'}, f"OBJ export failed: {e}")
            return {'CANCELLED'}

        prefs = context.preferences.addons[__name__].preferences
        script = bpy.path.abspath(prefs.converter_script.strip())
        if not os.path.exists(script):
            self.report({'ERROR'}, f"Converter script not found.")
            return {'CANCELLED'}

        # Ensure palette is saved to disk if any changes were made
        save_palette_to_disk(prefs)

        # 2. Main Conversion Wrapper
        cmd = [python_bin, script, obj_path, out_dir]
        if self.scale_mode == 'MANUAL': cmd += ['--scale', str(self.scale_factor)]
        else: cmd += ['--target-size', str(self.target_size)]
        if not self.centre_model: cmd += ['--no-center']
        
        if self.skip_grit: cmd += ['--skip-grit']
        elif self.grit_flags: cmd += ['--grit-flags', self.grit_flags]
        
        if prefs.mapping_file: 
            cmd += ['--mapping', bpy.path.abspath(prefs.mapping_file)]
            
        if self.inject_tiles:
            cmd += [
                '--tiles', 
                str(self.tile_start_x), str(self.tile_start_z), 
                str(self.tile_cols), str(self.tile_rows)
            ]

        self.report({'INFO'}, f"Running: {' '.join(cmd)}")
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
            if result.returncode != 0:
                self.report({'ERROR'}, f"Converter failed:\n{result.stderr[-500:]}")
                print("CONVERTER STDERR:", result.stderr)
                return {'CANCELLED'}
            print(result.stdout)
        except subprocess.TimeoutExpired:
            self.report({'ERROR'}, "Converter timed out.")
            return {'CANCELLED'}
        except Exception as e:
            self.report({'ERROR'}, f"Could not run converter: {e}")
            return {'CANCELLED'}

        self.report({'INFO'}, f"Done! Files written to {out_dir}")
        return {'FINISHED'}

# ── Registration ───────────────────────────────────────────────────────────────

def menu_func_export(self, context):
    self.layout.operator(NDS_OT_ExportEnvironment.bl_idname, text="NDS Environment (.h)")

classes = (
    NDSCollisionColor,
    NDS_UL_PaletteList,
    NDS_OT_LoadPalette,
    NDS_OT_SavePalette,
    NDS_OT_AddPaletteItem,
    NDS_OT_RemovePaletteItem,
    NDS_OT_SetBrushColor,
    NDS_PT_PaletteLegend_Image,
    NDS_PT_PaletteLegend_3D,
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