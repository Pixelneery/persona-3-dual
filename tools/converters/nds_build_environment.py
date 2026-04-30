import subprocess
import sys
import os
import re
import shutil
import argparse
from . import obj2environment

def find_grit():
    g = shutil.which('grit')
    if g: return g
    for c in ['/opt/devkitpro/tools/bin/grit',
              os.path.expanduser('~/devkitPro/tools/bin/grit'),
              'C:/devkitPro/tools/bin/grit.exe']:
        if os.path.exists(c): return c
    return None

def convert(input_file, output_dir, config):
    base_name = re.sub(r'[^a-zA-Z0-9_]', '_', os.path.splitext(os.path.basename(input_file))[0])
    
    # 1. Generate the header/assembly logic using obj2environment
    print(f"\n{'─'*60}\n  OBJ → NDS display list header\n{'─'*60}")
    obj2environment.convert(input_file, output_dir, config)
    
    # 2. Extract grit flags and run
    skip_grit = config.get("skip_grit", False)
    if skip_grit:
        print("\n[SKIP] GRIT (--skip-grit)")
        return
        
    tex_list = os.path.join(output_dir, f'{base_name}_textures.txt')
    if not os.path.exists(tex_list):
        print(f"\n[WARN] No texture list at {tex_list} — skipping GRIT.")
        return

    grit = find_grit()
    if not grit:
        print("\n[WARN] grit not found — install devkitPro and add tools/bin to PATH.")
        return

    grit_flags = config.get("grit_flags", "-ftc -fh -gb -gB16 -pu16")
    flags = grit_flags.split()
    
    pngs = [l.strip() for l in open(tex_list) if l.strip() and not l.startswith('#')]

    for png in pngs:
        if not os.path.exists(png):
            print(f"  [WARN] PNG not found: {png}"); continue
        stem = re.sub(r'[^a-zA-Z0-9_]', '_', os.path.splitext(os.path.basename(png))[0])
        
        cmd = [grit, png] + flags + ['-o', os.path.join(output_dir, stem)]
        print(f"\n{'─'*60}\n  GRIT: {os.path.basename(png)} → {stem}.h + {stem}.s\n  $ {' '.join(str(c) for c in cmd)}\n{'─'*60}")
        
        result = subprocess.run(cmd, text=True)
        if result.returncode != 0:
            print(f"\n[FAIL] GRIT conversion failed for {png}")
            sys.exit(result.returncode)

    if os.path.exists(tex_list):
        os.remove(tex_list)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='NDS environment build pipeline')
    parser.add_argument('input')
    parser.add_argument('output_dir')
    parser.add_argument('--scale',          type=float, default=None)
    parser.add_argument('--target-size',    type=float, default=4.0)
    parser.add_argument('--no-center',      action='store_true')
    parser.add_argument('--source-blender', action='store_true')
    parser.add_argument('--mapping',        type=str, default=None)
    parser.add_argument('--skip-grit',      action='store_true')
    parser.add_argument('--grit-flags',     type=str, default='-ftc -fh -gb -gB16 -pu16')
    args = parser.parse_args()

    cli_config = {
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender,
        "mapping": args.mapping,
        "skip_grit": args.skip_grit,
        "grit_flags": args.grit_flags
    }
    
    convert(args.input, args.output_dir, cli_config)