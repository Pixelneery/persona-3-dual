# Persona 3 Dual Asset Compilation Pipeline

This project uses a master dispatcher script (`tools/build_asset.py`) orchestrated by the Makefile to compile all assets dynamically. Instead of relying on gigantic strings of shell flags or embedding parameters in filenames, we store compiler overrides in `.build.json` "sidecar" files.

## How the Toolchain Works

When you type `make` in your terminal, the build system discovers all `.zip`, `.obj`, `.png`, and `.mp4` raw assets. 

For each asset, the `build_asset.py` dispatcher:
1. Locates the file (e.g., `assets/models/akihiko.zip`).
2. Checks if an identical `.build.json` file sits next to it (e.g., `assets/models/akihiko.build.json`).
3. Uses the file extension (or the `"asset_type"` key) to load the correct converter script.
4. Passes the JSON dictionary to the converter module.

## How to Make a `.build.json` Sidecar

A `.build.json` file is just a simple JSON file that overrides default build configurations. 

### 1. Basic Structure
The naming convention is identical to the source asset:
* Asset: `assets/environments/iwatodai_dorm/iwatodai_dorm.obj`
* JSON:  `assets/environments/iwatodai_dorm/iwatodai_dorm.build.json`

### 2. Available Configuration Flags

Here is an example `.build.json` translating common CLI flags. If a flag is omitted, the compiler defaults to standard behaviour.

```json
{
    "asset_type": "obj2model",        // (Optional) Forces a specific script
    "target_size": 4.0,               // (--target-size 4.0) Auto-scales longest edge
    "scale_factor": 0.054,            // (--scale 0.054) Only applied if "scale_mode" is "manual"
    "center": true,                   // Centers origin (use false for --no-center)
    "source_blender": true,           // (--source-blender) Reorients Z-up to Y-up
    "texsize": [64, 64],              // (--texsize 64 64) Force 64x64 texture mappings
    "crop": [0, 0, 44, 33],           // (--crop 0 0 44 33) Specifically for texture2collision
    "skip_grit": false,               // (--skip-grit) For environments
    "grit_flags": "-ftc -fh -gb -gB16 -pu16",
    "bits": 16,                       // (--bits 16) For Video encoding
    "fps": 24                         // (--fps 24) For Video encoding
}