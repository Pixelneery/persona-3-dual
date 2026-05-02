import sys
import os
import re
import argparse
from . import obj2environment

def convert(input_file, output_dir, config):
    base_name = re.sub(r'[^a-zA-Z0-9_]', '_', os.path.splitext(os.path.basename(input_file))[0])
    
    print(f"\n{'─'*60}\n  OBJ → NDS display list binary\n{'─'*60}")
    obj2environment.convert(input_file, output_dir, config)
    
    # 2. Hand off Texture Processing to the Native Makefile
    tex_list = os.path.join(output_dir, f'{base_name}_textures.txt')
    if not os.path.exists(tex_list):
        return

    # devkitARM makefiles use -fts automatically, so we only need the pixel formatting flags
    grit_flags = config.get("grit_flags", "-gb -gB16 -p!")
    
    pngs = [l.strip() for l in open(tex_list) if l.strip() and not l.startswith('#')]

    for png in pngs:
        if not os.path.exists(png):
            print(f"  [WARN] PNG not found: {png}"); continue
            
        # overwrite the .grit sidecar file to sync with JSON flags
        grit_file = os.path.splitext(png)[0] + '.grit'
        print(f"  [GRIT] Generating {os.path.basename(grit_file)} for Make pipeline...")
        with open(grit_file, 'w') as f:
            f.write(grit_flags)

    # Cleanup the temporary txt file
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
    parser.add_argument('--grit-flags',     type=str, default='-gb -gB16 -p!')
    parser.add_argument('--rgba',           action='store_true', help='Use GL_RGBA for all textures.')
    parser.add_argument('--rgba-list',      type=str, default='', help='Comma-separated texture names.')
    parser.add_argument('--color',          nargs=3, type=int, metavar=('R', 'G', 'B'), default=[255, 255, 255])
    args = parser.parse_args()

    cli_config = {
        "scale": args.scale,
        "target_size": args.target_size,
        "no_center": args.no_center,
        "source_blender": args.source_blender,
        "mapping": args.mapping,
        "skip_grit": args.skip_grit,
        "grit_flags": args.grit_flags,
        "rgba": args.rgba,
        "rgba_list": args.rgba_list,
        "color": args.color
    }
    
    convert(args.input, args.output_dir, cli_config)