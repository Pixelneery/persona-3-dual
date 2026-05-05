#!/usr/bin/env python3
"""
jmap_to_h.py — Convert a .jmap collision map to a C header file.

Usage:
    python3 jmap_to_h.py <input.jmap> <output.h>

Tile tokens:
    w = NO_COLLISION  (0)  walkable
    c = COLLISION     (1)  wall
    s = SAVE          (2)  save point
    p = PREV_SCENE    (3)  warp to previous scene
    e = NEXT_SCENE    (4)  warp to next scene
    a = reserved      (0)  treated as walkable
"""

import sys
import os
import re

TILE_MAP = {
    'w': 0,  # NO_COLLISION
    'c': 1,  # COLLISION
    's': 2,  # SAVE
    'p': 3,  # PREV_SCENE
    'e': 4,  # NEXT_SCENE
    'a': 0,  # reserved — walkable
}

def parse_jmap(path):
    rows = []
    audio = None
    with open(path, 'r') as f:
        for raw in f:
            line = raw.split('#')[0].strip()  # strip comments
            # check for @audio directive in the comment before stripping
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
                    raise ValueError(f"Unknown tile token '{tok}' in {path}")
                row.append(TILE_MAP[tok])
            rows.append(row)
    return rows, audio

def validate(rows, path):
    if not rows:
        raise ValueError(f"No map data found in {path}")
    width = len(rows[0])
    for i, row in enumerate(rows):
        if len(row) != width:
            raise ValueError(
                f"Row {i} has {len(row)} tiles, expected {width} (in {path})"
            )
    return len(rows), width

def to_header(rows, height, width, stem, audio):
    guard = re.sub(r'[^A-Z0-9]', '_', stem.upper()) + '_H'
    define_prefix = re.sub(r'[^A-Z0-9]', '_', stem.upper())

    lines = []
    if audio:
        lines.append(f'#define {define_prefix}_MUSIC "{audio}"')
        lines.append('')
    lines.append(f'// Auto-generated from {stem}.jmap — do not edit by hand')
    lines.append(f'#ifndef {guard}')
    lines.append(f'#define {guard}')
    lines.append('')
    lines.append('#include <stdint.h>')
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
        print(f'Usage: {sys.argv[0]} <input.jmap> <output.h>', file=sys.stderr)
        sys.exit(1)

    jmap_path = sys.argv[1]
    out_path  = sys.argv[2]

    stem = os.path.splitext(os.path.basename(jmap_path))[0]

    rows, audio = parse_jmap(jmap_path)
    height, width = validate(rows, jmap_path)
    header = to_header(rows, height, width, stem, audio)

    os.makedirs(os.path.dirname(out_path) or '.', exist_ok=True)
    with open(out_path, 'w') as f:
        f.write(header)

    print(f'  JMAP  {os.path.basename(jmap_path)} -> {os.path.basename(out_path)} ({width}x{height})')

if __name__ == '__main__':
    main()
