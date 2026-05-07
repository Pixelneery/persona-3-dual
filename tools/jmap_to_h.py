#!/usr/bin/env python3
"""
jmap_to_h.py — Convert a .jmap collision map to a C header file.

Usage:
    python3 jmap_to_h.py <input.jmap> <output.h>

See tile_map.json for mapping values.
"""

import json
import sys
import re
from pathlib import Path

# Load the tile map from the JSON file
script_dir = Path(__file__).resolve().parent
json_path = script_dir / "tile_map.json"

try:
    with json_path.open('r') as f:
        data = json.load(f)
    TILE_MAP = data['TILE_MAP']
except FileNotFoundError:
    print(f"Error: Could not find {json_path}", file=sys.stderr)
    sys.exit(1)

def parse_jmap(path_obj):
    rows = []
    audio = None
    with path_obj.open('r') as f:
        for raw in f:
            line = raw.split('#')[0].strip()
            comment = raw.partition('#')[2].strip()
            if comment.startswith('@audio'):
                parts = comment.split(None, 1)
                if len(parts) == 2:
                    audio = parts[1].strip()
            if not line:
                continue
            tokens = [t.strip() for t in line.split(',') if t.strip()]
            if not tokens:
                continue
            row = []
            for tok in tokens:
                if tok not in TILE_MAP:
                    raise ValueError(f"Unknown tile token '{tok}' in {path_obj.name}")
                row.append(TILE_MAP[tok])
            rows.append(row)
    return rows, audio

def validate(rows, path_obj):
    if not rows:
        raise ValueError(f"No map data found in {path_obj.name}")
    width = len(rows[0])
    for i, row in enumerate(rows):
        if len(row) != width:
            raise ValueError(
                f"Row {i} has {len(row)} tiles, expected {width} (in {path_obj.name})"
            )
    return len(rows), width

def to_header(rows, height, width, stem, audio):
    guard = re.sub(r'[^A-Z0-9]', '_', stem.upper()) + '_H'
    define_prefix = re.sub(r'[^A-Z0-9]', '_', stem.upper())

    lines = []
    lines.append(f'// Auto-generated from {stem}.jmap - do not edit by hand')
    lines.append('#include <stdint.h>')
    lines.append('')
    if audio:
        lines.append(f'#define {define_prefix}_MUSIC "{audio}"')
        lines.append('')
    lines.append(f'#ifndef {guard}')
    lines.append(f'#define {guard}')
    lines.append('')
    lines.append(f'#define {define_prefix}_MAP_WIDTH  {width}')
    lines.append(f'#define {define_prefix}_MAP_HEIGHT {height}')
    lines.append('')
    lines.append(
        f'static const uint8_t {stem}_collision_map'
        f'[{define_prefix}_MAP_HEIGHT][{define_prefix}_MAP_WIDTH] = {{'
    )
    for r, row in enumerate(rows):
        comma = ',' if r < height - 1 else ''
        values = ', '.join(str(v) for v in row)
        lines.append(f'    {{{values}}}{comma}')
    lines.append('};')
    lines.append('')
    lines.append(f'#endif')
    return '\n'.join(lines) + '\n'

def main():
    if len(sys.argv) != 3:
        # Show just the script name in help
        script_name = Path(sys.argv[0]).name
        print(f'Usage: {script_name} <input.jmap> <output.h>', file=sys.stderr)
        sys.exit(1)

    # Convert strings from sys.argv into Path objects immediately
    jmap_path = Path(sys.argv[1])
    out_path  = Path(sys.argv[2])

    # get filename
    stem = jmap_path.stem

    rows, audio = parse_jmap(jmap_path)
    height, width = validate(rows, jmap_path)
    header = to_header(rows, height, width, stem, audio)

    # Create the parent directory if it doesn't exist
    out_path.parent.mkdir(parents=True, exist_ok=True)
    
    # Write the file directly using the Path object
    out_path.write_text(header)

    print(f'  JMAP  {jmap_path.name} -> {out_path.name} ({width}x{height})')

if __name__ == '__main__':
    main()