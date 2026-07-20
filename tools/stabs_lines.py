#!/usr/bin/env python3
"""Filter cc1 -gstabs output down to line records modern gdb can read.

gcc 2.8.1 emits STABS debug info, but modern gdb's STABS reader crashes
(internal-error in create_range_type) on this compiler's builtin type
idioms — self-referential integer ranges (``unsigned int:t4=r4;0;-1;``) and
``size;0`` floats. Source-level stepping does not need those type
definitions, though: the line-number records do.

This keeps only the stabs gdb needs for address→source mapping and drops
every type/variable stab that trips the crash:

  keep  N_SO   (100) / N_SOL (132)  source file
        N_FUN  (36)                 function
        N_SLINE(68, via .stabn)     line → address

  drop  N_LSYM (128), N_GSYM (32), N_STSYM (38), N_RSYM (64), N_PSYM (160),
        and every other .stabs/.stabn — the type and variable descriptors.

The result assembles to .stab/.stabstr sections whose N_SLINE addresses are
exact (adding -gstabs was verified not to change .text), so gdb shows C
source lines for matched functions. Types come from the PCSX-Redux Typed
Debugger import instead (tools/redux_typed_debugger.py).

    tools/stabs_lines.py < in.s > out.s
    tools/stabs_lines.py --input in.s --output out.s
"""

from __future__ import annotations

import argparse
import re
import sys
from typing import Iterable

# .stabs "str",TYPE,other,desc,value  — keep these type codes.
KEEP_STABS_CODES = {100, 132, 36}
_STABS = re.compile(r"^\s*\.stabs\b[^,]*,\s*(\d+),")
_STABN = re.compile(r"^\s*\.stabn\b\s*(\d+),")
N_SLINE = 68


def filter_lines(lines: Iterable[str]) -> list[str]:
    out = []
    for line in lines:
        stabs = _STABS.match(line)
        if stabs:
            if int(stabs.group(1)) in KEEP_STABS_CODES:
                out.append(line)
            continue
        stabn = _STABN.match(line)
        if stabn:
            if int(stabn.group(1)) == N_SLINE:
                out.append(line)
            continue
        out.append(line)
    return out


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=argparse.FileType("r"), default=sys.stdin)
    parser.add_argument("--output", type=argparse.FileType("w"), default=sys.stdout)
    args = parser.parse_args(argv)
    args.output.writelines(filter_lines(args.input))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
