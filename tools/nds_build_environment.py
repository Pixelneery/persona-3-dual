#!/usr/bin/env python3
"""
nds_build_environment.py  —  One-command build pipeline for NDS environments
For the Persona 3 Dual project.

Runs the full pipeline on an OBJ:
  1. obj2nds_environment.py  → modelName_env.h + modelName_textures.txt
  2. GRIT on every listed texture  → textureName.h + textureName.s

Place this script in your project root alongside obj2nds_environment.py.

Usage:
    python nds_build_environment.py input.obj output_dir/
    python nds_build_environment.py input.obj output_dir/ --target-size 4.0
    python nds_build_environment.py input.obj output_dir/ --scale 0.054
    python nds_build_environment.py input.obj output_dir/ --mapping mat_map.json
    python nds_build_environment.py input.obj output_dir/ --grit-flags "-ftc -fh -gb -gB16 -pu16"
    python nds_build_environment.py input.obj output_dir/ --skip-grit

The --mapping flag points to a JSON file for models whose MTL has no map_Kd entries
(e.g. models exported from XPS/XNALara tools):
  {
    "5_BaseWeight(1)_1_0_0":  "textures/f007_009_01.png",
    "5_BaseWeight(2)_1_0_0":  "textures/f007_009_02.png",
    ...
  }
"""

import subprocess
import sys
import os
import re
import argparse

HERE = os.path.dirname(os.path.abspath(__file__))
CONVERTER = os.path.join(HERE, 'obj2nds_environment.py')


def run(cmd, desc):
    print(f"\n{'─'*60}")
    print(f"  {desc}")
    print(f"  $ {' '.join(str(c) for c in cmd)}")
    print(f"{'─'*60}")
    result = subprocess.run(cmd, text=True)
    if result.returncode != 0:
        print(f"\n[FAIL] {desc}")
        sys.exit(result.returncode)


def find_grit():
    """Find the grit executable on PATH or in common NDS SDK locations."""
    import shutil
    g = shutil.which('grit')
    if g:
        return g
    candidates = [
        '/opt/devkitpro/tools/bin/grit',
        os.path.expanduser('~/devkitPro/tools/bin/grit'),
        'C:/devkitPro/tools/bin/grit.exe',
    ]
    for c in candidates:
        if os.path.exists(c):
            return c
    return None


def main():
    parser = argparse.ArgumentParser(
        description='Full NDS environment build pipeline: OBJ → .h + texture .h files'
    )
    parser.add_argument('input',        help='Input .obj file')
    parser.add_argument('output_dir',   help='Output directory')
    parser.add_argument('--scale',      type=float, default=None)
    parser.add_argument('--target-size',type=float, default=4.0)
    parser.add_argument('--no-center',  action='store_true')
    parser.add_argument('--mapping',    type=str,   default=None,
                        help='JSON file mapping material names to PNG paths')
    parser.add_argument('--skip-grit',  action='store_true',
                        help='Skip the GRIT step (just run the OBJ converter)')
    parser.add_argument('--grit-flags', type=str,
                        default='-ftc -fh -gb -gB16 -pu16',
                        help='GRIT command-line flags (default: -ftc -fh -gb -gB16 -pu16)')
    parser.add_argument('--source-blender', action='store_true',
                        help='Treat the input OBJ as Blender Z-up coordinates and convert to NDS Y-up')
    parser.add_argument('--tiles',      type=float, nargs=4, metavar=('START_X', 'START_Z', 'COLS', 'ROWS'),
                        default=None, help='Injects collision macros into header (e.g. --tiles 0 0 64 64)')
    args = parser.parse_args()

    obj_path   = os.path.abspath(args.input)
    output_dir = os.path.abspath(args.output_dir)
    os.makedirs(output_dir, exist_ok=True)

    # ── Step 1: OBJ → .h ─────────────────────────────────────────────────────
    cmd = [sys.executable, CONVERTER, obj_path, output_dir]
    if args.scale is not None:
        cmd += ['--scale', str(args.scale)]
    else:
        cmd += ['--target-size', str(args.target_size)]
    if args.no_center:
        cmd += ['--no-center']
    if args.mapping:
        cmd += ['--mapping', args.mapping]
    if args.source_blender:
        cmd += ['--source-blender']
    if args.tiles:
        cmd += ['--tiles'] + [str(t) for t in args.tiles]

    run(cmd, 'Converting OBJ to NDS display list header')

    # ── Step 2: GRIT on all textures ──────────────────────────────────────────
    if args.skip_grit:
        print("\n[SKIP] GRIT step skipped (--skip-grit)")
        print("Run GRIT manually on each PNG listed in the _textures.txt file.")
        return

    base_name = re.sub(r'[^a-zA-Z0-9_]', '_',
                       os.path.splitext(os.path.basename(obj_path))[0])
    tex_list_path = os.path.join(output_dir, f'{base_name}_textures.txt')

    if not os.path.exists(tex_list_path):
        print(f"\n[WARN] No texture list found at {tex_list_path}. "
              f"Skipping GRIT.")
        return

    grit = find_grit()
    if grit is None:
        print("\n[WARN] 'grit' not found on PATH or in devkitPro. "
              "Skipping GRIT step.")
        print("Install devkitPro and add its tools/bin to PATH, then run GRIT "
              "manually on each PNG in the _textures.txt file.")
        return

    grit_flags = args.grit_flags.split()

    png_paths = []
    with open(tex_list_path) as f:
        for line in f:
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            png_paths.append(line)

    if not png_paths:
        print("\n[INFO] No textures to convert.")
        return

    print(f"\nFound {len(png_paths)} texture(s) to convert with GRIT.")
    for png_path in png_paths:
        if not os.path.exists(png_path):
            print(f"  [WARN] PNG not found: {png_path} — skipping")
            continue

        # GRIT outputs files in the current directory; we want them in output_dir
        # Use -o flag to set output filename stem
        stem    = re.sub(r'[^a-zA-Z0-9_]', '_',
                         os.path.splitext(os.path.basename(png_path))[0])
        out_stem = os.path.join(output_dir, stem)

        cmd = [grit, png_path] + grit_flags + ['-o', out_stem]
        run(cmd, f'GRIT: {os.path.basename(png_path)} → {stem}.h + {stem}.s')

    print(f"\n{'='*60}")
    print(f"  BUILD COMPLETE")
    print(f"  Output: {output_dir}/")
    print(f"  Include in your NDS project:")
    print(f"    #include \"{base_name}_env.h\"")
    for png_path in png_paths:
        stem = re.sub(r'[^a-zA-Z0-9_]', '_',
                      os.path.splitext(os.path.basename(png_path))[0])
        print(f"    #include \"{stem}.h\"")
    print(f"{'='*60}\n")


if __name__ == '__main__':
    main()
