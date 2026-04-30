# Persona 3 Dual: Animated Model Pipeline (OUTDATED README)

This document details the automated workflow for converting rigged 3D models and their animations into Nintendo DS-compatible hierarchical display lists and keyframe arrays.

Unlike static environments, animated models are exported as a collection of isolated `.obj` files (one per bone/node) tied together by a central `.json` file containing the skeletal hierarchy and animation tracks.

---

## 1. Project Structure & Naming Conventions

The build system relies on a strict input-output mapping and a specific naming convention to auto-detect texture sizes.

* **Input Directory:** `assets/models/<model_name>/`
* **Required Files:** The extracted contents of your exporter's `.zip` file (the `.json` file and all `_nX.obj` files) MUST be placed here.
* **Texture Scaling (CRITICAL):** The NDS requires texture sizes to be powers of two. To automatically pass the correct scale to the compiler, append the resolution to your filename: 
  * `player_64x64.json`
  * If omitted (e.g., `enemy.json`), the Makefile falls back to the default `MODEL_TEXSIZE` (32x32).

**Output:** The pipeline parses the JSON and outputs a single lightweight header at `source/models/<model_name>.h`.

---

## 2. The Automated Make Workflow

The `Makefile` watches your `assets/models/` directory and will automatically recompile headers if the `.json` or `.obj` files change.

### Make Commands

| Command | Action |
| :--- | :--- |
| `make` | Builds the entire ROM, triggering model conversion if any JSON files have been modified. |
| `make models` | Runs the conversion pipeline strictly for the animated models without building the rest of the ROM. |
| `make clean` | Wipes the generated `source/models/` directory. |

---

## 3. Authoring Tools

You can build and animate your models using either Blender or Blockbench. Both plugins output a standardized `.zip` file containing your `.json` and per-node `.obj` files. **You must extract this `.zip` into your `assets/models/<name>/` folder before building.**

### A. Blender Plugin (`nds_model_exporter.py`)
Best for complex armatures, weighted meshes, and NLA-track animations.

**Setup:**
1. Go to **Edit > Preferences > Add-ons > Install**.
2. Select `nds_model_exporter.py` and enable it.

**Modeling Conventions:**
* **Bones = Nodes:** One armature bone becomes one NDS node.
* **Vertex Groups:** Geometry is bound to a bone via Vertex Groups. The vertex group name **must exactly match** the bone name. Unassigned vertices are ignored.
* **Animations:** The plugin automatically exports the active Action, plus all stashed NLA tracks as separate animations.
* **Textures:** Apply your single texture image to the active material. The plugin will attempt to auto-detect its size to append to the filename.

**Exporting:**
* Select your Armature.
* Go to **File > Export > NDS Model (.zip)**.

### B. Blockbench Plugin (`nds_model_exporter.js`)
Best for rigid, low-poly, Minecraft-style modeling and strict hierarchical animations.

**Setup:**
1. Go to **File > Plugins > Load Plugin from File**.
2. Select `nds_model_exporter.js`.

**Modeling Conventions:**
* **Groups = Nodes:** Every Group/Folder in your outliner becomes an NDS node. Nested groups become child nodes.
* **Geometry:** Any Cubes or Meshes inside a Group belong to that node.
* **Animations:** All animations in the Blockbench Animation tab are exported. Time is automatically converted from seconds to 60 FPS keyframes.

**Exporting:**
* Go to **File > Export > Export NDS Model (ZIP)**.

---

## 4. Command Line Interface (CLI)

The pipeline is powered by `obj2model.py`. You can run it manually for debugging or custom automation.

**Basic Usage:**
```bash
python3 tools/obj2model.py assets/models/player/player_64x64.json source/models/player.bin [options]
```

**CLI Arguments:**

| Argument | Action |
| :--- | :--- |
| `--texsize W H` | Explicitly defines the width and height of the texture (e.g., `--texsize 64 64`). |
| `--color R G B` | Applies a flat vertex color (0-255) to the entire model (e.g., `--color 255 128 0`). |

*(Note: The script outputs a `.h` file despite the `.bin` output target name in the CLI, due to internal handling of the JSON conversion vs standard OBJ conversion).*

---

## 5. C++ Integration

Once the build is complete, integrating the animated model into your C++ engine is entirely handled by the `AnimationController`.

**1. Include the Header**
```cpp
#include "models/player.h" // Matches your JSON filename without the _WxH suffix
```

**2. Initialize the Controller**
```cpp
AnimationController myPlayer;

void MyView::Init() {
    // Pass the controller to the auto-generated Load function
    LoadModel_player(myPlayer);
    
    // Use the auto-generated Enums for type-safe animation triggering
    myPlayer.set(MODEL_PLAYER_WALK, true); // true = loop
    myPlayer.play();
}
```

**3. Update and Render**
```cpp
void MyView::Update() {
    // Tick the animation forward
    myPlayer.update();
    
    glPushMatrix();
        // Bind the model's texture
        glBindTexture(GL_TEXTURE_2D, playerTextureId);
        
        // Render the hierarchical display lists
        myPlayer.render();
    glPopMatrix(1);
}
```