# NDS Model Exporter Toolchain

This toolchain provides a complete workflow for exporting animated, hierarchical 3D models from **Blender** or **Blockbench** into a highly optimized, hardware-ready Nintendo DS binary format. 

The pipeline bridges the gap between modern 3D software and NDS hardware by baking skeletal meshes into per-bone `.obj` files, extracting animation keyframes, and compiling them into pre-calculated display lists and transformation tracks.

---

## Installation

### 1. Command Line Converter (`obj2model.py`)
This standalone script converts the exported `.json` hierarchy into the final `.bin` and `.h` files.
1. Requires Python 3. No external dependencies.
2. Place `obj2model.py` in your project's `tools/` directory.

### 2. Blender Plugin (`nds_model_exporter.py`)
1. Open Blender (3.0+ or 4.x).
2. Go to **Edit > Preferences > Add-ons**.
3. Click **Install...** and select `nds_model_exporter.py`.
4. Check the box to enable **Import-Export: NDS Model Exporter**.

### 3. Blockbench Plugin (`nds_model_exporter.js`)
1. Open Blockbench.
2. Go to **File > Plugins**.
3. Click the **Load Plugin from File** icon (folder icon).
4. Select `nds_model_exporter.js`.

---

## How to Use

### Step 1A: Exporting from Blender
1. **Setup your Rig:** Use an **Armature** with Mesh children. Your meshes must be weighted to specific bones using **Vertex Groups** named exactly after the bones.
2. **Setup your Material:** Your mesh must use a single image texture node linked to the Base Color.
3. Select your Armature and parented Meshes.
4. Go to **File > Export > NDS Model (.zip)**.
5. Save the file. The plugin extracts animation actions/NLA strips and isolates the geometry per bone based on vertex weight.

### Step 1B: Exporting from Blockbench
1. **Setup your Hierarchy:** Use **Groups** (folders) to act as your "bones". Place cubes and meshes inside these groups.
2. Create your animations in the Animate tab.
3. Go to **File > Export > Export NDS Model (ZIP)**.
4. The plugin automatically converts the Blockbench groups and keyframes (rotation/position) into the NDS hierarchy.

### Step 2: Compiling the Binary
Both plugins output a `.zip` file containing a `modelName.json` and a collection of `modelName_nX.obj` files.

1. **Extract the `.zip` file** to a working directory.
2. Run `obj2model.py` against the generated `.json` file:
   ```bash
   # From Blockbench
   python obj2model.py my_model.json my_model.bin --texsize 128 128
   
   # From Blender (Requires the --source-blender flag for coordinate fixing)
   python obj2model.py my_blender_model.json my_blender_model.bin --source-blender --texsize 256 256
   ```

---

## Output Files

The final `obj2model.py` compilation yields:
* **`[model_name].bin`**: A raw binary (`MDL1` magic) containing the node hierarchy, pre-calculated global pivot points, packed FIFO display lists per node, and optimized keyframe animation tracks.
* **`[model_name].h`**: A C++ header containing a generated `enum` representing your animation tracks by name (e.g., `MODEL_MYMODEL_WALK`, `MODEL_MYMODEL_RUN`), allowing you to trigger them safely in your engine.

---

## Quick Reference: CLI Flags

| Flag | Description | Default |
| :--- | :--- | :--- |
| `input` | Path to the `.json` (from the extracted ZIP) or a single `.obj`. | *Required* |
| `output` | Path to the destination `.bin` file. | *Required* |
| `--texsize W H` | Provides texture width and height to correctly calculate hardware UV coordinates (`t16`). | `None` |
| `--color R G B` | Applies a flat vertex color (0-255) to the entire model. | `None` |
| `--scale` | Manually forces a scale multiplier on all vertex positions and animation translations. | Auto |
| `--target-size` | Auto-scales the model so its largest bounding box dimension matches this value. | `4.0` |
| `--no-center` | Prevents the script from automatically shifting the model's origin to its bottom-center. | Centered |
| `--source-blender`| **Required for Blender exports.** Applies Z-up to Y-up conversions, flips UV V-axis, and adjusts scale logic (Blockbench divides pivots by 16.0, Blender uses 1.0). | Disabled |

### Technical Notes & Quirks
* **Keyframe Optimization:** The `obj2model.py` script automatically strips redundant linear keyframes to save NDS memory. If three consecutive frames share the exact same rotation and position, the middle frame is deleted.
* **Animation Baking:** Ensure your Blender animations are baked into standard Actions or NLA tracks. Procedural constraints/IK might require visual keying before export to accurately capture world transforms.
* **Hardware Precision:** Vertex coordinates and positions are clamped and packed into standard NDS 16-bit fixed-point (`v16`). Rotations use a 16-bit angle format (0 to 32767 mapped across 360 degrees).