# NDS Model Exporter — Pipeline README

**For the Persona 3 Dual project.**  
Covers the Blockbench plugin, the Blender add-on, and the Python converter that consumes their output.

---

## Table of Contents

1. [How the Pipeline Works](#1-how-the-pipeline-works)
2. [Blockbench Plugin](#2-blockbench-plugin)
   - [Installation](#installation)
   - [Model Setup Rules](#model-setup-rules-blockbench)
   - [Exporting](#exporting-blockbench)
3. [Blender Add-on](#3-blender-add-on)
   - [Installation](#installation-1)
   - [Model Setup Rules](#model-setup-rules-blender)
   - [Animation Setup Rules](#animation-setup-rules-blender)
   - [Exporting](#exporting-blender)
4. [Python Converter](#4-python-converter-obj2ndspy)
5. [Output Format Reference](#5-output-format-reference)
6. [Constraints & Known Limitations](#6-constraints--known-limitations)

---

## 1. How the Pipeline Works

Both the Blockbench plugin and the Blender add-on produce the **same ZIP file**. The Python converter then reads that ZIP and produces a C header (`.h`) ready for the NDS.

```
Blockbench ──┐
             ├──► modelName.zip ──► obj2nds.py ──► modelName.h
Blender ─────┘
```

The ZIP always contains:
- `modelName.json` — node hierarchy + animation keyframes
- `modelName_n0.obj`, `modelName_n1.obj`, … — one OBJ per node/bone

The model name is derived automatically from the project/armature name and the detected texture size, e.g. `player_64x64`.

---

## 2. Blockbench Plugin

### Installation

1. Open Blockbench.
2. Go to **File → Plugins → Load Plugin from File**.
3. Select `nds_model_exporter.js`.
4. The export action appears under **File → Export → Export NDS Model (ZIP)**.

### Model Setup Rules (Blockbench)

**Groups = Nodes**

Every exportable part of the model must be inside a **Group**. Groups map directly to NDS nodes. The group hierarchy (parent/child nesting) becomes the NDS parent/child hierarchy.

- A group with no geometry children exports as an empty node (useful for pivot-only bones).
- Geometry that is not inside any group is ignored.
- Group names become the `name` field in the JSON. Avoid special characters — anything that isn't a letter or number is replaced with `_`.

**Geometry**

- Cubes and meshes are both supported.
- A cube or mesh is assigned to whichever group directly contains it. There is no weight painting — every element belongs to exactly one group.
- Elements can be toggled non-exportable in Blockbench's outliner; the plugin respects that flag.

**Texture**

- The plugin reads the resolution of the **first texture** in the texture list.
- The texture dimensions are appended to the model name (e.g. `player_64x64`).
- Only power-of-two sizes are valid for NDS: 8, 16, 32, 64, 128, 256, 512, 1024.
- If no texture is present, no suffix is added.

**Pivot / Origin**

- Each group's **pivot point** in Blockbench becomes the `origin` field in the JSON, which the converter uses as the NDS bone pivot.
- Set pivots carefully — they directly affect how the node rotates at runtime.

### Exporting (Blockbench)

1. Name your project (the base of the model name comes from here).
2. Go to **File → Export → Export NDS Model (ZIP)**.
3. Choose a save location. The file is named `modelName.zip` automatically.

---

## 3. Blender Add-on

### Installation

1. Go to **Edit → Preferences → Add-ons → Install**.
2. Select `nds_model_exporter_blender.py`.
3. Enable the add-on (search for "NDS").
4. The export option appears under **File → Export → NDS Model (.zip)**.

### Model Setup Rules (Blender)

The add-on uses **standard Blender rigging conventions** with a few specific constraints described below.

---

**Scene structure**

Your scene needs:
- One **Armature** object — this is the skeleton. Its name becomes the base model name.
- One or more **Mesh** objects that are either:
  - Parented to the armature with an **Armature modifier**, or
  - Directly parented to the armature object.

If there are multiple armatures in the scene, select the one you want to export before running the exporter. If nothing is selected, it picks the first armature it finds.

---

**Bones = Nodes**

Every bone in the armature becomes one NDS node. The bone's parent in the armature hierarchy becomes its `parentId` in the JSON (-1 for root bones).

- Bone names become node names. Avoid special characters — they are replaced with `_`.
- The **bone head position** (in armature local space) is used as the node's `origin`/pivot.
- Bone ordering in the JSON matches the order bones were created in the armature. This order is stable but not alphabetical. The NDS node IDs (0, 1, 2, …) follow this creation order.
- Empty bones (no geometry assigned to them) export as empty nodes — this is fine and useful for pivot hierarchies.

---

**Vertex groups — the critical rule**

Geometry is assigned to bones through **vertex groups**. The vertex group name must **exactly match the bone name**.

> **This is the most important setup requirement.** Without correct vertex groups, geometry will be silently missing from the export.

Rules the exporter enforces:
- A vertex is assigned to a bone only if its weight in that bone's vertex group is **greater than 0.5**.
- Each vertex is assigned to at most one bone (the one with weight > 0.5). Blended weights across multiple bones are not supported — the NDS has no concept of smooth skinning.
- A polygon (face) is only exported into a bone's OBJ if **all of its vertices** are assigned to that bone. Faces that cross a bone boundary are dropped entirely.

**Practical implication:** You must paint weights in a "rigid" style — each vertex fully belongs to one bone. Do not use Blender's automatic weights or smooth weight painting as-is, since those spread weight across multiple bones.

**How to set up rigid weights quickly:**

1. In Object Mode, select your mesh, then Shift-click the armature, and press `Ctrl+P → Armature Deform` (this creates the vertex groups automatically from bone names).
2. Go into Weight Paint mode.
3. For each bone, paint its region with weight = 1.0, and ensure no vertex is painted on more than one bone with weight > 0.5.
4. Alternatively, use `Ctrl+P → Armature Deform with Envelope Weights` and clean up the result, or assign vertex groups manually in Edit Mode using the vertex group panel.

---

**Faces: triangles and quads only**

- Triangles and quads are exported natively (the NDS supports both `GL_TRIANGLES` and `GL_QUADS`).
- N-gons (5+ vertices) are automatically triangulated during export.
- For best results, keep your mesh as quads and tris only.

---

**Texture**

- The exporter searches all materials on all mesh objects for the first **Image Texture node** in the node tree.
- That image's pixel dimensions are appended to the model name (e.g. `player_64x64`).
- Only one texture is supported. Multi-texture models are not supported.
- The texture image must be loaded in Blender (not just referenced by path). Packed images work fine.
- Only power-of-two sizes are valid for NDS: 8, 16, 32, 64, 128, 256, 512, 1024.

---

**Scale — must be applied**

The exporter reads geometry in world space and transforms it into armature local space. If your mesh or armature has a non-unit scale (i.e. the scale shown in the N panel is not 1, 1, 1), the exported geometry will be wrong.

Before exporting, apply all transforms:
1. Select all objects (armature + meshes).
2. Press `Ctrl+A → All Transforms`.

---

**Coordinate space**

Blender uses Z-up, Y-forward. The NDS uses a different convention. The exporter does **not** perform any axis conversion — it exports coordinates as-is in Blender's armature local space. Build your model in whatever orientation it needs to be in the game, or handle axis conversion in your NDS rendering code.

---

### Animation Setup Rules (Blender)

The exporter reads keyframe data from Blender Actions. It does **not** bake — it reads only frames where you have explicitly placed keyframes.

**What is exported:**
- The **active Action** on the armature object (the one currently shown in the Action Editor or Dope Sheet).
- All Actions stored as **NLA strips** on NLA tracks of the armature.
- Each Action becomes one named animation in the JSON. The animation name is sanitized (special characters → `_`).

**What is not exported:**
- Motion produced by constraints (IK, Copy Rotation, etc.) unless you bake it first.
- Motion produced by drivers.
- Interpolated values between keyframes — only the keyframed frames are captured.

**If you use constraints or IK:** Bake the action before exporting.
1. With the armature selected in Pose Mode, go to **Pose → Animation → Bake Action**.
2. Check "Only Selected Bones" if needed, set the frame range, enable "Clear Constraints", and bake.
3. The result is a new action with explicit keyframes on every frame. Export that.

**Rotation mode — important**

The exporter decomposes bone rotations as **Euler XYZ**. If a bone uses Quaternion rotation mode, Blender converts it automatically, but you may see unexpected flipping between keyframes in complex rotations (gimbal lock). To avoid this, set all animated bones to **Euler XYZ** rotation mode before animating:
1. Select all bones in Pose Mode (`A`).
2. In the N panel (Item tab), set Rotation Mode to **XYZ Euler**.

**Animation duration**

The `duration` field in the JSON is the number of frames in the Action's frame range (end − start + 1). Set your Action's frame range in the Dope Sheet or Action Editor to exactly what you need.

**Frame timing**

The converter uses frame numbers directly as NDS timing units. The NDS runs at 60 fps. Set your scene frame rate to 60 fps (`Output Properties → Frame Rate`) so that keyframe numbers correspond directly to NDS frames.

### Exporting (Blender)

1. Name your armature object — this becomes the base model name.
2. Apply all transforms (`Ctrl+A → All Transforms` on all objects).
3. Make sure your scene frame rate is 60 fps.
4. Select the armature (optional, but ensures the right one is picked if you have multiple).
5. Go to **File → Export → NDS Model (.zip)**.
6. Choose a save location and click **Export NDS Model**.

---

## 4. Python Converter (`obj2nds.py`)

The converter reads the ZIP output and produces a C++ header for the NDS.

**Convert a model ZIP:**
```bash
python obj2nds.py model_64x64.json model_64x64.bin
```
The `.json` file must be extracted from the ZIP first, with all OBJ files in the same directory.

Or pass options directly:
```bash
python obj2nds.py model_64x64.json model_64x64.bin --texsize 64 64
python obj2nds.py model_64x64.json model_64x64.bin --texsize 64 64 --color 255 128 0
```

**Options:**

| Flag | Description |
|---|---|
| `--texsize W H` | Override texture dimensions (used for UV scaling). If the model name includes `_WxH`, this is auto-parsed from the filename and you usually don't need this flag. |
| `--color R G B` | Apply a flat vertex color (0–255 per channel) to all geometry. |

**Output:**

The converter produces a `.h` file (same name as the output `.bin` with `.h` extension) containing:
- An enum of animation names for type-safe indexing (`MODEL_PLAYER_IDLE`, etc.)
- Inline display list arrays per bone (`static const u32 player_dl_0[]`)
- Keyframe arrays per animation track
- A `LoadModel_player(AnimationController& ctrl)` function to call at runtime

Include the `.h` in your NDS project and call the load function once during initialization.

---

## 5. Output Format Reference

### JSON structure

```json
{
  "nodes": [
    {
      "id": 0,
      "parent": -1,
      "name": "root",
      "obj": "player_64x64_n0.obj",
      "origin": [0.0, 0.0, 0.0]
    },
    {
      "id": 1,
      "parent": 0,
      "name": "head",
      "obj": "player_64x64_n1.obj",
      "origin": [0.0, 8.0, 0.0]
    }
  ],
  "animations": {
    "idle": {
      "duration": 60,
      "tracks": {
        "1": [
          { "time": 0,  "rot": [0, 0, 0],   "pos": [0, 0, 0] },
          { "time": 30, "rot": [0, 15, 0],  "pos": [0, 0, 0] },
          { "time": 60, "rot": [0, 0, 0],   "pos": [0, 0, 0] }
        ]
      }
    }
  }
}
```

- `id` — zero-based node index (matches the `_nX` suffix in the OBJ filename)
- `parent` — `id` of parent node, or `-1` for root
- `origin` — pivot point in model local space (Blockbench units or Blender units)
- `duration` — animation length in frames (at 60 fps = NDS frames)
- `tracks` — keyed by node `id` as a string; only animated nodes are included
- `rot` — Euler XYZ rotation in degrees
- `pos` — position offset in model units

---

## 6. Constraints & Known Limitations

| Constraint | Applies To | Notes |
|---|---|---|
| One texture per model | Both | First texture found is used; multi-texture not supported |
| Power-of-two texture sizes only | Both | NDS hardware requirement |
| Rigid weights only (no blend skinning) | Blender | Vertices with weight ≤ 0.5 on all bones are ignored |
| Faces crossing bone boundaries are dropped | Blender | All verts of a face must belong to the same bone |
| No constraint/IK/driver baking | Blender | Bake actions manually before export if used |
| Euler XYZ rotation only | Blender | Set bone rotation mode to XYZ Euler to avoid gimbal issues |
| Scale must be applied | Blender | Run `Ctrl+A → All Transforms` before export |
| No axis conversion | Blender | Export is in Blender's coordinate space; handle in engine if needed |
| No scale keyframes | Both | NDS animation has no scale channel |
| Bone ordering = creation order | Blender | Node IDs depend on the order bones were added to the armature |
| N-gons are triangulated | Blender | Quads and tris are kept as-is; 5+ vert faces are split |
| Active Action + NLA strips only | Blender | Actions not assigned to NLA and not active are not exported |