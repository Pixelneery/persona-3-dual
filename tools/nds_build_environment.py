#!/usr/bin/env python3
"""
nds_build_environment.py  —  One-command NDS environment build pipeline
For the Persona 3 Dual project.

  1. obj2nds_environment.py  ->  model_env.h + model_textures.txt
  2. GRIT on every texture   ->  texture.h  + texture.s

Usage:
    python nds_build_environment.py input.obj output_dir/
    python nds_build_environment.py input.obj output_dir/ --source-blender
    python nds_build_environment.py input.obj output_dir/ --target-size 4.0
    python nds_build_environment.py input.obj output_dir/ --skip-grit
"""

import subprocess, sys, os, re, argparse, shutil

HERE      = os.path.dirname(os.path.abspath(__file__))
CONVERTER = os.path.join(HERE, 'obj2nds_environment.py')


def run(cmd, desc):
    print(f"\n{'─'*60}\n  {desc}\n  $ {' '.join(str(c) for c in cmd)}\n{'─'*60}")
    result = subprocess.run(cmd, text=True)
    if result.returncode != 0:
        print(f"\n[FAIL] {desc}")
        sys.exit(result.returncode)


def find_grit():
    g = shutil.which('grit')
    if g: return g
    for c in ['/opt/devkitpro/tools/bin/grit',
              os.path.expanduser('~/devkitPro/tools/bin/grit'),
              'C:/devkitPro/tools/bin/grit.exe']:
        if os.path.exists(c): return c
    return None


def main():
    p = argparse.ArgumentParser(
        description='NDS environment build pipeline: OBJ -> .h + textures')
    p.add_argument('input')
    p.add_argument('output_dir')
    p.add_argument('--scale',          type=float, default=None)
    p.add_argument('--target-size',    type=float, default=4.0)
    p.add_argument('--no-center',      action='store_true')
    p.add_argument('--source-blender', action='store_true')
    p.add_argument('--mapping',        type=str, default=None)
    p.add_argument('--skip-grit',      action='store_true')
    p.add_argument('--grit-flags',     type=str,
                   default='-ftc -fh -gb -gB16 -pu16')
    args = p.parse_args()

    obj_path   = os.path.abspath(args.input)
    output_dir = os.path.abspath(args.output_dir)
    os.makedirs(output_dir, exist_ok=True)
    base_name  = re.sub(r'[^a-zA-Z0-9_]', '_',
                        os.path.splitext(os.path.basename(obj_path))[0])

    # ── Step 1: OBJ → env header ───────────────
    cmd = [sys.executable, CONVERTER, obj_path, output_dir]
    if args.scale is not None: cmd += ['--scale',       str(args.scale)]
    else:                      cmd += ['--target-size', str(args.target_size)]
    if args.no_center:         cmd += ['--no-center']
    if args.source_blender:    cmd += ['--source-blender']
    if args.mapping:           cmd += ['--mapping', args.mapping]

    run(cmd, 'OBJ → NDS display list header')

    # ── Step 2: GRIT ─────────────────────────────────────────────────────────
    if args.skip_grit:
        print("\n[SKIP] GRIT (--skip-grit)")
        return

    tex_list = os.path.join(output_dir, f'{base_name}_textures.txt')
    if not os.path.exists(tex_list):
        print(f"\n[WARN] No texture list at {tex_list} — skipping GRIT."); return

    grit = find_grit()
    if not grit:
        print("\n[WARN] grit not found — install devkitPro and add tools/bin to PATH."); return

    flags = args.grit_flags.split()
    pngs  = [l.strip() for l in open(tex_list)
             if l.strip() and not l.startswith('#')]

    for png in pngs:
        if not os.path.exists(png):
            print(f"  [WARN] PNG not found: {png}"); continue
        stem = re.sub(r'[^a-zA-Z0-9_]', '_',
                      os.path.splitext(os.path.basename(png))[0])
        run([grit, png] + flags + ['-o', os.path.join(output_dir, stem)],
            f'GRIT: {os.path.basename(png)} → {stem}.h + {stem}.s')

     # Clean up the temporary texture list
    if os.path.exists(tex_list):
        os.remove(tex_list)
        
    print(f"\n{'='*60}\n  BUILD COMPLETE  →  {output_dir}/")
    print(f"    #include \"{base_name}_env.h\"")
    for png in pngs:
        print(f"    #include \"{re.sub(chr(91) + r'^a-zA-Z0-9_' + chr(93), '_', os.path.splitext(os.path.basename(png))[0])}.h\"")
    print(f"{'='*60}\n")


if __name__ == '__main__':
    main()