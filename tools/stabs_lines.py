#!/usr/bin/env python3
"""Patch cc1 -gstabs output so modern gdb reads it — keeping types and locals.

gcc 2.8.1 emits STABS debug info, but modern gdb's STABS reader
internal-errors in create_range_type on three of its type encodings, whose
index/range comes out zero-length:

  1. self-referential unsigned ranges     ``unsigned int:t4=r4;0;-1;``
  2. float/double size ranges             ``float:t12=r1;4;0;``
  3. arrays with a type-0 index           ``…=ar0;0;N;elem``

Dropping the type definitions avoids the crash but leaves locals with no type
(``<incomplete type>`` — no value in the debugger). Instead this *patches*
only those three forms so the whole thing loads: gdb then has real types, and
VSCode's Locals shows variables with values.

  1. unsigned ``0;-1`` → a width-correct upper bound (by builtin name;
     other unsigned ranges fall back to the 32-bit max).
  2. float/double ``;size;0;`` → an int32 range. Float locals therefore show
     their raw 32-bit value, not a decimal float — a display-only compromise
     (the PCSX-Redux Typed Debugger import has exact types); no crash.
  3. array index type 0 → 1 (``int``), which is defined and non-zero-length,
     preserving the element type and bounds.

-gstabs was verified not to change .text, and this only rewrites .stab
strings, so addresses stay exact.

    tools/stabs_lines.py < in.s > out.s
    tools/stabs_lines.py --input in.s --output out.s
"""

from __future__ import annotations

import argparse
import re
import sys
from typing import Iterable

# (pattern, replacement) applied in order to each .stab line.
_PATCHES = [
    (re.compile(r"(unsigned char:t\d+=r\d+);0;-1;"), r"\1;0;255;"),
    (re.compile(r"(short unsigned int:t\d+=r\d+);0;-1;"), r"\1;0;65535;"),
    (re.compile(r"((?:long )?unsigned int:t\d+=r\d+);0;-1;"), r"\1;0;037777777777;"),
    (re.compile(r"(long long (?:unsigned )?int:t\d+=r\d+);0;-1;"),
     r"\1;0;01777777777777777777777;"),
    # Any remaining self-referential unsigned range → 32-bit max.
    (re.compile(r";0;-1;"), r";0;037777777777;"),
    # Float/double size range (high == 0) → int32 range.
    (re.compile(r"(=r\d+);\d+;0;"), r"\1;-2147483648;2147483647;"),
    # Array indexed by the undefined type 0 → int (type 1).
    (re.compile(r"=ar0;"), r"=ar1;"),
]


def fixup_stabs(lines: Iterable[str]) -> list[str]:
    out = []
    for line in lines:
        if ".stab" in line:
            for pattern, replacement in _PATCHES:
                line = pattern.sub(replacement, line)
        out.append(line)
    return out


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=argparse.FileType("r"), default=sys.stdin)
    parser.add_argument("--output", type=argparse.FileType("w"), default=sys.stdout)
    args = parser.parse_args(argv)
    args.output.writelines(fixup_stabs(args.input))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
