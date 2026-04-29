# NDS Environment Pipeline — README

**Persona 3 Dual project — Environment model toolchain.**

This document covers the complete workflow for getting 3D environment models into the game — from source files in Blockbench or Blender, through the converter scripts, and into working NDS C++ code.

---

## Table of Contents

1. [Overview & Concepts](#1-overview--concepts)
2. [File Reference](#2-file-reference)
3. [Installation & Prerequisites](#3-installation--prerequisites)
4. [Workflow A — Blockbench (legacy, single texture)](#4-workflow-a--blockbench-legacy-single-texture)
5. [Workflow B — Blender (current, multi-texture)](#5-workflow-b--blender-current-multi-texture)
6. [Workflow C — XPS / Legacy OBJ (no MTL texture mapping)](#6-workflow-c--xps--legacy-obj-no-mtl-texture-mapping)
7. [Converter Reference](#7-converter-reference)
8. [Using the Generated Header in C++](#8-using-the-generated-header-in-c)
9. [NDS Constraints to Keep in Mind](#9-nds-constraints-to-keep-in-mind)
10. [Troubleshooting](#10-troubleshooting)

---

## 1. Overview & Concepts

### What the pipeline does

The NDS GPU does not load 3D files at runtime. Instead, geometry is pre-compiled into a **display list** — a binary blob of GPU commands that replays vertex and UV data directly into the hardware FIFO. This pipeline converts standard OBJ files into those display lists and wraps them in a C++ header your scene code can include and call.

### The old workflow (Blockbench, manual)

```
Blockbench → OBJ (manually scaled small) + single PNG
    → obj2nds.py  →  model.bin
    → GRIT on PNG  →  texture.h
    → manually wire up glBindTexture + glCallList in C++ code
```

Every step after Blockbench was manual. Scaling was done by hand in Blockbench. Texture binding was written by hand in each view file. If the model had more than one texture, the process was repeated manually for each one.

### The new workflow (Blender, automated)

```
Blender (multi-texture scene)
    → File > Export > NDS Environment (.h)   [Blender add-on]
          OR
    → standard OBJ export from Blender
          ↓
    nds_build_environment.py  [one command]
          ↓
    modelName_env.h          ← include this in your project
    textureName.h / .s × N   ← GRIT output, include these too
```

Scaling is computed automatically. All textures are handled in one pass. The output header contains `Load_`, `Draw_`, and `Delete_` functions — you call those three functions in your view Init/Update/Cleanup, nothing else.

---

## 2. File Reference

| File | Purpose |
|---|---|
| `obj2nds_environment.py` | Core converter. OBJ + MTL → `modelName_env.h` + `modelName_textures.txt`. |
| `nds_build_environment.py` | One-command build script. Runs the converter then runs GRIT on every texture. |
| `nds_environment_exporter_blender.py` | Blender add-on. Exports OBJ+MTL from Blender and calls the converter — one button click. |
| `obj2nds.py` | Original single-OBJ converter (unchanged). Still used for character models. |

All scripts must live in the same folder. `nds_build_environment.py` and `nds_environment_exporter_blender.py` both locate `obj2nds_environment.py` by looking in their own directory.

---

## 3. Installation & Prerequisites

### Python

Python 3.8 or newer. Install the Pillow library for automatic texture size detection:

```bash
pip install Pillow
```

If Pillow is not installed the converter still works but it cannot read PNG dimensions automatically, so texture sizes default to 8×8 and `glTexImage2D` calls in the generated header will use `TEXTURE_SIZE_8`. You would need to correct these by hand. Just install Pillow.

### devkitPro / GRIT

GRIT is the NDS texture converter that ships with devkitPro. The build script finds it automatically at the standard install paths:

- Linux/Mac: `/opt/devkitpro/tools/bin/grit`
- Windows: `C:/devkitPro/tools/bin/grit.exe`
- Or anywhere on your `PATH`

If GRIT is not found, the build script prints a warning and skips the texture step. You can then run GRIT manually on each PNG listed in `modelName_textures.txt`, or add devkitPro's `tools/bin` to your system PATH.

### Blender add-on

Requires Blender 3.x or 4.x.

1. Open Blender.
2. Edit → Preferences → Add-ons → Install.
3. Navigate to `nds_environment_exporter_blender.py` and select it.
4. Enable the add-on by ticking its checkbox.
5. The export option now appears under **File → Export → NDS Environment (.h)**.

The add-on also requires `obj2nds_environment.py` to be on your machine. You point the add-on at it once in the export dialog (see Section 5).

---

## 4. Workflow A — Blockbench (legacy, single texture)

This is the original workflow. Use it if you are working in Blockbench with a single texture sheet.

### Model setup in Blockbench

- Every piece of geometry must be inside a **Group**. Ungrouped geometry is ignored.
- The model uses a **single texture** applied to all faces. The texture file must be a power-of-two size (8, 16, 32, 64, 128, 256, 512, or 1024 pixels per side).
- The pivot point of each group is used as the NDS bone origin. Set these carefully — they affect how the node rotates at runtime.
- Name the project cleanly. The project name becomes the base of all output filenames. Special characters are replaced with `_`.

### Exporting from Blockbench

**For character/animated models**, use the NDS Model Exporter plugin (see the character model README). This produces a ZIP with per-bone OBJs and a JSON hierarchy.

**For environment models (this workflow)**, export a plain OBJ manually:

1. File → Export → Export OBJ.
2. Export to a working folder alongside your texture PNG.
3. Note the scale — Blockbench models are already small (the dorm building is about 2.75 NDS units wide). If you are re-exporting an existing model, match the scale it had before.

### Converting with obj2nds_environment.py

```bash
python obj2nds_environment.py iwatodai_dorm.obj output/ --target-size 4.0
```

This produces `iwatodai_dorm_env.h` and `iwatodai_dorm_textures.txt` in `output/`.

Or use the one-command build script which also runs GRIT:

```bash
python nds_build_environment.py iwatodai_dorm.obj output/ --target-size 4.0
```

### Important: scale for Blockbench models

Blockbench models are already in NDS-friendly units. If you use `--target-size 4.0` on a model that is already ~2.75 units wide, the auto-scaler will enlarge it slightly (to 4.0). If you want to preserve the original scale exactly, use `--scale 1.0` instead.

---

## 5. Workflow B — Blender (current, multi-texture)

This is the preferred workflow for all new environment assets. It handles models with many textures automatically, applies correct NDS scaling, and produces ready-to-include headers in one step.

### Material setup in Blender — the one rule that matters

Each material must have exactly **one Image Texture node** in its node tree. It does not need to be connected to anything specific — the add-on searches the entire node tree for any Image Texture node and uses the first one it finds.

The standard setup that works:

```
Image Texture node  →  Base Color input of Principled BSDF
```

One material = one texture = one draw call on the NDS. If multiple materials reference the same image file, the converter merges their geometry into a single draw call automatically.

Materials without any Image Texture node will export with no UV mapping and render as a solid colour (white by default). A warning is printed.

### Geometry setup in Blender

**Apply all transforms before exporting.** The add-on exports in world space. If your mesh or any parent object has a non-unit scale or unexpected rotation, the exported geometry will be wrong.

```
Select all objects → Ctrl+A → All Transforms
```

**UV unwrap everything.** Every face on every mesh must have UV coordinates. Faces without UVs export without UV data and will render without textures.

**Face types.** Triangles and quads are both supported by the NDS. N-gons (5 or more vertices) are automatically triangulated during export by the add-on. For best results, clean up n-gons before exporting.

**Multiple mesh objects are fine.** The add-on collects all mesh objects in the scene (or all selected ones if you tick Selection Only) and merges them into one OBJ. Objects are grouped by material, not by mesh object name.

### Texture setup in Blender

- Textures must be power-of-two in both dimensions (8, 16, 32, 64, 128, 256, 512, 1024).
- Non-power-of-two textures will still export but the converter will round up to the next valid size when generating `glTexImage2D` calls, which will cause incorrect UV mapping.
- The NDS has 512KB of texture VRAM (banks A+B). Keep total texture memory in mind — 25 textures at 64×64 in RGBA16 is 25 × 8KB = 200KB, which fits. 25 textures at 256×256 would be 25 × 128KB = 3.2MB, which does not.
- Textures can be packed into the Blender file or referenced as external files. The add-on handles both — packed images are saved to the output directory automatically.

### Exporting from Blender using the add-on

1. File → Export → NDS Environment (.h).
2. Choose the output path. Name the file after your model (e.g. `iwatodai.h`). The `_env` suffix is added automatically, so the output will be `iwatodai_env.h`.
3. Set the options in the left panel:

   **NDS Scale Mode — Auto** (recommended): The longest axis of the model is scaled to "Target Size" NDS units. The default of `4.0` is a good starting point for a building-sized environment. Increase it if your scene feels too cramped in the camera, decrease it if objects are clipping near/far planes. The NDS camera in the demo scene has `zNear=0.1` and `zFar=40`, so all visible geometry must fit within 40 NDS units of the camera.

   **NDS Scale Mode — Manual**: Multiply all vertex coordinates by an explicit factor. Use this if you have established a known scale for your project and want to match it exactly. The reference value for the existing dorm model is approximately `0.054` (Blender units to NDS units for that asset).

   **Centre Model**: Translates the model so it is centred on X and Z at the world origin, and sits on Y=0. Leave this on unless you are placing the model at a specific offset in your C++ code.

   **Selection Only**: Export only the objects you have selected in the viewport. Useful if your scene file contains multiple environments or scratch geometry you do not want to include.

   **Converter Script**: Click the folder icon and navigate to `obj2nds_environment.py`. You only need to set this once per machine — Blender saves it with your preferences. If left blank, the add-on writes the OBJ and MTL but skips conversion.

4. Click **Export NDS Environment**.

The add-on writes the OBJ+MTL, calls the converter, and prints progress to the Blender system console (Window → Toggle System Console on Windows).

### Exporting from Blender without the add-on

If you prefer using Blender's built-in OBJ exporter:

1. File → Export → Wavefront (.obj).
2. Enable **Apply Modifiers**, **Include UVs**, **Write Materials**, **Triangulate Faces**.
3. Set Path Mode to **Copy** and click the clipboard icon next to it — this embeds textures alongside the OBJ.
4. Export.
5. Run the build script on the resulting OBJ:

```bash
python nds_build_environment.py model.obj output/
```

The MTL produced by Blender's exporter will have correct `map_Kd` entries pointing to the PNG files, so no mapping file is needed.

### After exporting — what you get

Running the build script (or the add-on with a converter script set) produces a directory containing:

```
output/
  modelName_env.h          ← main header, include this in your project
  modelName_textures.txt   ← list of PNGs processed by GRIT (for reference)
  textureName.h            ← GRIT output, one per unique texture
  textureName.s            ← GRIT assembly, one per unique texture
```

Add all `.h` and `.s` files to your Makefile's source list. Then follow Section 8 for the C++ integration.

---

## 6. Workflow C — XPS / Legacy OBJ (no MTL texture mapping)

Some OBJ files — particularly those exported from XPS/XNALara tools — have an MTL that lists material names but has no `map_Kd` lines. The converter cannot determine which texture to use for each material without this information.

For these files, provide a **mapping JSON file** that associates each material name to a PNG path.

### Creating the mapping file

Create a `.json` file with this structure:

```json
{
    "5_BaseWeight(1)_1_0_0":   "textures/f007_009_01.png",
    "5_BaseWeight(2)_1_0_0":   "textures/f007_009_02.png",
    "5_BaseWeight(3)_1_0_0":   "textures/f007_009_03.png",
    "7_BaseWeight(7)_1_0_0":   "textures/f007_009_07.png"
}
```

The keys are the exact material names from the OBJ's `usemtl` lines. You can find all material names by running:

```bash
grep "^usemtl" yourmodel.obj | sort -u
```

Paths are relative to the mapping JSON file's location, or you can use absolute paths.

Materials not listed in the JSON are exported without texture mapping (solid colour with a warning).

### Running the converter with a mapping file

```bash
python nds_build_environment.py iwatodai.obj output/ --mapping mat_map.json
```

Or directly:

```bash
python obj2nds_environment.py iwatodai.obj output/ --mapping mat_map.json
```

### Moving away from XPS

The XPS workflow requires maintaining the mapping JSON manually every time textures change. Strongly recommended: import the XPS asset into Blender, assign the textures properly to materials (Image Texture node in each material), and switch to Workflow B. This is a one-time setup cost that eliminates the mapping file entirely.

---

## 7. Converter Reference

### obj2nds_environment.py

```
python obj2nds_environment.py INPUT.obj OUTPUT_DIR/ [options]

Arguments:
  input          Path to the .obj file. The .mtl must be alongside it.
  output_dir     Directory to write the .h and .txt files into.

Options:
  --target-size N    Auto-scale so the longest axis = N NDS units. Default: 4.0.
  --scale N          Explicit scale multiplier. Overrides --target-size.
  --no-center        Do not translate the model to the origin.
  --mapping FILE     JSON file mapping material names to PNG paths.
```

Outputs:
- `modelName_env.h` — the C++ header (see Section 8)
- `modelName_textures.txt` — list of PNG absolute paths, one per line, for GRIT

### nds_build_environment.py

```
python nds_build_environment.py INPUT.obj OUTPUT_DIR/ [options]

Arguments:
  input          Path to the .obj file.
  output_dir     Directory for all output files.

Options:
  --target-size N    Passed through to obj2nds_environment.py. Default: 4.0.
  --scale N          Explicit scale multiplier.
  --no-center        Skip centering.
  --mapping FILE     JSON material-to-texture mapping file.
  --skip-grit        Run only the converter; skip the GRIT texture step.
  --grit-flags STR   Override GRIT flags. Default: "-ftb -fh -gb -gB16 -pu16"
```

This script expects `obj2nds_environment.py` to be in the same directory as itself.

### Blender add-on panel options

| Option | Description |
|---|---|
| NDS Scale Mode | Auto (recommended) or Manual |
| Target Size | Auto mode only. Longest axis = this many NDS units. Default 4.0. |
| Scale Factor | Manual mode only. Direct vertex multiplier. Default 0.054. |
| Centre Model | Translate to origin on X/Z, sit on Y=0. Default on. |
| Selection Only | Export selected objects only. Default off (all meshes). |
| Converter Script | Path to `obj2nds_environment.py`. Required to produce the `.h`. |

---

## 8. Using the Generated Header in C++

The `modelName_env.h` header is self-contained. It contains three inline functions and requires no other auto-generated includes beyond the GRIT texture headers.

### What the header provides

```cpp
// Texture slot enum — for indexing into your textureIds array
enum modelName_TexSlot {
    MODELNAME_TEX_TEXTURE0 = 0,
    MODELNAME_TEX_TEXTURE1 = 1,
    MODELNAME_TEX_COUNT = 2
};

// Call once in Init(), after VRAM is set up
void Load_modelName(int textureIds[N], const u8* bitmaps[N]);

// Call each frame in Update()
void Draw_modelName(const int textureIds[N]);

// Call in Cleanup()
void Delete_modelName(int textureIds[N]);
```

### Complete integration example

In your view header (`MyView.h`):

```cpp
#include "models/iwatodai_env.h"

// One integer per texture slot
static int iwatodaiTextureIds[IWATODAI_TEX_COUNT];
```

In your view source (`MyView.cpp`):

```cpp
// Include GRIT-generated bitmap headers (one per texture)
#include "f007_009_01.h"
#include "f007_009_02.h"
// ... etc for all textures listed in iwatodai_textures.txt

void MyView::Init() {
    // ... your existing VRAM / glInit setup ...

    // Load the environment
    const u8* bitmaps[IWATODAI_TEX_COUNT] = {
        f007_009_01Bitmap,
        f007_009_02Bitmap,
        // ... same order as the IWATODAI_TEX_* enum values
    };
    Load_iwatodai(iwatodaiTextureIds, bitmaps);
}

ViewState MyView::Update() {
    // ... camera and input ...

    glPushMatrix();
        Draw_iwatodai(iwatodaiTextureIds);
    glPopMatrix(1);

    glFlush(0);
    return ViewState::KEEP_CURRENT;
}

void MyView::Cleanup() {
    Delete_iwatodai(iwatodaiTextureIds);
    // ... other cleanup ...
}
```

### Bitmap variable naming convention

GRIT names the bitmap variable after the input filename. For `f007_009_01.png`, GRIT produces `f007_009_01Bitmap`. Special characters in the filename (hyphens, spaces) become underscores. If unsure, open the GRIT-generated `.h` file and look for the `extern const u8` declaration.

### Finding the correct bitmap order

The `Load_` function takes bitmaps in the order of the `_TexSlot` enum, which matches the order textures appear in the display list (i.e. the order they were first encountered in the OBJ from top to bottom). The converter prints each slot's texture filename when it runs:

```
[f007_009_01.png] 1240 faces, tex 64x64, 14880 FIFO words   ← slot 0
[f007_009_02.png] 890 faces, tex 64x64, 10680 FIFO words    ← slot 1
```

Alternatively, look at the `_TexSlot` enum in the generated header — the names correspond directly to the PNG filenames.

---

## 9. NDS Constraints to Keep in Mind

These are hardware limits that cannot be worked around in software.

### Vertex coordinate range

NDS vertex coordinates are stored as 12.4 fixed-point numbers (`v16`), giving a range of roughly -8.0 to +7.999 NDS units. The auto-scaler keeps models within this range by default. If you use `--scale` manually, verify that no vertex exceeds this range, or geometry will silently wrap/clamp.

### Texture dimensions

Textures must be power-of-two in both width and height. Valid values: 8, 16, 32, 64, 128, 256, 512, 1024. Non-power-of-two textures must be padded or resized before conversion — the NDS will not render them correctly.

### Texture memory budget

The demo scene allocates VRAM banks A and B for textures: 256KB + 256KB = 512KB total. RGBA16 (16-bit colour, the format GRIT produces with `-gB16`) uses 2 bytes per pixel:

| Texture size | Memory |
|---|---|
| 64×64 | 8 KB |
| 128×128 | 32 KB |
| 256×256 | 128 KB |

With 512KB available, you can hold 64 textures at 64×64, or 4 textures at 256×256, or any combination. Plan your texture atlas sizes accordingly.

### Polygon types

The NDS supports only triangles and quads. The converter handles both natively. N-gons are fan-triangulated. Prefer quads for slightly lower FIFO word count (one `GL_BEGIN QUADS` per face instead of one `GL_BEGIN TRIANGLES`).

### No smooth skinning for environments

Environments are static geometry stored in a single display list. There is no per-vertex weighting, bone influence, or deformation. This is correct and intentional — environments should be rigid.

### Camera clip planes

The demo scene uses `gluPerspective(55, 256/192, 0.1, 40)`. Any geometry further than 40 NDS units from the camera is clipped. Size and position your environment to fit within this range. If your scene is large, you may need to increase `zFar`, but this affects depth buffer precision — keep it as small as the scene allows.

---

## 10. Troubleshooting

**The generated header has `TEXTURE_SIZE_8` for all textures.**
Pillow is not installed. Run `pip install Pillow` and re-run the converter. If Pillow is installed and the problem persists, check that the PNG files are located in the same directory as the OBJ when the converter runs — it looks for textures relative to the OBJ path.

**"WARNING: no texture mapping for material X"**
The MTL file has no `map_Kd` entry for that material. This happens with XPS-exported files. Either add `map_Kd` entries to the MTL manually, or create a mapping JSON file and pass it with `--mapping`. See Section 6.

**Geometry is missing in the game.**
Check for faces that have vertices with no UV mapping. The converter skips UV commands for those vertices but still exports the geometry — the face may render at UV (0,0) and blend into the wrong part of the texture, appearing invisible or wrong. Ensure every face has a UV in Blender before exporting.

**The model is huge / tiny in the game.**
The auto-scale `--target-size 4.0` targets the longest axis. If your model has a large empty bounding box (e.g. a displaced object far from the origin), the bounds inflate and the model comes out smaller than expected. Centre your model in Blender before exporting, or use `--scale` with an explicit value you have verified in game.

**The model is offset — not where I placed it in the C++ code.**
By default the converter centres the model on X and Z and sits it on Y=0. If your C++ code positions the model at a specific offset based on the original coordinates, pass `--no-center` to preserve the original world-space origin. The Blender add-on has a "Centre Model" toggle for the same purpose.

**Blender add-on: "No mesh objects found to export."**
If "Selection Only" is ticked and nothing is selected in the viewport, the export has nothing to work with. Either deselect "Selection Only" to export all meshes, or select the objects you want first.

**Blender add-on: converter ran but no `.h` file appeared.**
Check the Blender system console (Window → Toggle System Console on Windows) for error output from the converter. The most common cause is the converter script path being wrong — re-check that "Converter Script" points to `obj2nds_environment.py` and that the file exists at that path.

**GRIT is not found.**
Install devkitPro and ensure its `tools/bin` is on your system PATH, or run GRIT manually on each PNG listed in `modelName_textures.txt`. The GRIT flags to use are: `-ftb -fh -gb -gB16 -pu16`.

**Blender exported the OBJ but materials have wrong/no textures.**
Open the MTL file that was written alongside the OBJ and check for `map_Kd` lines. If they are missing, the material in Blender has no Image Texture node. Open the Shader Editor for that material and confirm there is an Image Texture node with an image assigned to it.