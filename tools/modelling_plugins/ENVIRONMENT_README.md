# Persona 3 Dual: Environment Pipeline (OUTDATED README)

This document details the automated workflow for converting 3D models and textures into Nintendo DS-compatible Assembly (`.s`) and C++ headers (`.h`). 

This pipeline is strictly decoupled from the physics engine; it handles visuals, display lists, and texture VRAM loading only.

---

## 1. Project Structure

The build system relies on a strict input-output directory mapping.

* **Input Directory:** `assets/environments/<environment_name>/`
* **Required Files:** Your `.obj` mesh, `.mtl` material file, and all associated `.png` textures MUST be grouped together in this folder.
* **Output Directory:** `source/environments/<environment_name>/` 

During compilation, the pipeline automatically generates the `_env.h` file, the raw `.s` display lists, and runs GRIT on all textures, placing the final results into the output directory.

---

## 2. The Automated Make Workflow

The easiest way to process environments is to let the `Makefile` handle it. The build system watches your asset folders and only recompiles when changes are detected.

### Make Commands

| Command | Action |
| :--- | :--- |
| `make` | Builds the entire ROM, triggering environment conversion if any `.obj` or `.png` files have been modified. |
| `make environments` | Runs the conversion pipeline strictly for the 3D environments without building the rest of the ROM. |
| `make clean` | Wipes the `source/environments/` directory to force a fresh conversion on the next build. |

### Make Overrides

By default, the Makefile passes `--source-blender` to the Python script to automatically fix the Z-up (Blender) to Y-up (NDS) coordinate mismatch. You can override this from the command line if using assets from different 3D software:

```bash
make ENV_FLAGS=""
```

---

## 3. Blender Plugin Setup & Usage

If you prefer to export directly from your 3D workspace to a custom directory, you can use the included Blender Addon (`nds_environment_exporter.py`).

### Installation

1.  Open Blender and navigate to **Edit > Preferences > Add-ons**.
2.  Click **Install** and select `nds_environment_exporter.py`.
3.  Enable the Add-on.
4.  Expand the Add-on preferences and set the **Build Script** path to the absolute location of `nds_build_environment.py` on your machine.

### Usage

1.  Select the objects you wish to export.
2.  Navigate to **File > Export > NDS Environment (.h)**.
3.  Configure your settings in the export window.
4.  Click Export. The plugin will silently run the CLI pipeline and generate the NDS files in your chosen directory.

### Plugin Settings

| Setting | Description |
| :--- | :--- |
| **Scale Mode** | Choose between `Auto Size` (scales the longest dimension to a target unit size) or `Manual Scale` (applies a flat multiplier). |
| **Centre Model** | Automatically centers the geometry around `(0,0,0)` in world space. |
| **Selection Only** | Exports only the actively highlighted meshes. |
| **Skip GRIT** | Generates the 3D display list but bypasses texture `.s` conversion. |

---

## 4. Command Line Interface (CLI)

The pipeline is powered by `nds_build_environment.py`, a wrapper that manages the OBJ converter and GRIT texture processor. You can run it manually for debugging or custom automation.

**Basic Usage:**
```bash
python3 tools/nds_build_environment.py input.obj output_dir/ [options]
```

**CLI Arguments:**

| Argument | Action |
| :--- | :--- |
| `--target-size FLOAT` | Auto-scales the model so its longest axis equals this value in NDS units (Default: 4.0). |
| `--scale FLOAT` | Applies an explicit, manual scale multiplier. Overrides `--target-size`. |
| `--no-center` | Prevents the script from translating the model to the origin. |
| `--source-blender` | Rotates the vertices from Blender's Z-up system to the NDS Y-up system. |
| `--mapping FILE` | Path to a JSON file explicitly mapping material names to PNG textures. |
| `--skip-grit` | Skips texture processing. |
| `--grit-flags STRING` | Custom flags passed to GRIT (Default: `-fts -fh -gb -gB16 -pu16`). |

---

## 5. C++ Integration

Once the build is complete, implementing the environment in your C++ View requires just a few lines of code.

**1. Include the Header**
```cpp
#include "environments/my_room/my_room_env.h"
```

**2. Initialize Memory**
```cpp
// Allocate an array for the OpenGL texture IDs
static int myRoomTextureIds[MY_ROOM_TEX_COUNT];

void MyView::Init() {
    // Group the GRIT-generated bitmaps in slot order
    const unsigned int* bitmaps[MY_ROOM_TEX_COUNT] = {
        floorBitmap, 
        wallsBitmap
    };
    
    // Load textures into VRAM
    Load_my_room(myRoomTextureIds, bitmaps);
}
```

**3. Render the Scene**
```cpp
void MyView::Update() {
    glPushMatrix();
        Draw_my_room(myRoomTextureIds);
    glPopMatrix(1);
}
```

**4. Cleanup**
```cpp
void MyView::Cleanup() {
    Delete_my_room(myRoomTextureIds);
}
```