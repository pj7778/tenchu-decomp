#!/usr/bin/env python3
"""Locate main.exe's functions inside the other five executables.

Tenchu links the same engine into six PS-EXEs. `trial.exe` (the Mission Editor)
is a near-complete relink of the game; `menu.exe`, `ending.exe` and `slps_019.01`
carry the engine core plus the whole statically-linked Sony SDK; `run.exe` is the
loader and shares little but the SDK.

Two modes:

  exact       byte-for-byte identical function bodies. Every immediate, jal target
              and gp-relative offset coincides, so a function we have already
              byte-matched in main.exe reproduces those bytes for free. Rare: the
              two builds put data at different addresses, so anything touching a
              global differs. Useful only as a drop-in list.

  normalized  (default) mask out the relocatable fields -- j/jal targets, lui
              immediates, and every I-type immediate -- before comparing. This
              finds the *same code* regardless of where the linker put things.
              It is the same normalisation `xbuildnames.py` uses against the demo.

The normalized mode doubles as a symbol-recovery tool: a unique hit gives that
function's address in the other exe. Hits are cross-checked for adjacency (a run
of functions landing end-to-end at consecutive addresses is very strong evidence
the boundaries are right), and ambiguous hits -- a normalized body occurring more
than once, which happens for small identical helpers -- are dropped.

    tools/xexe.py                       # summary table, all targets
    tools/xexe.py --target trial.exe    # per-function detail
    tools/xexe.py --target trial.exe --exact
    tools/xexe.py --target trial.exe --tsv reference/xexe-trial.tsv

Nothing here writes to config/. Minting config/symbols.<target>.txt from a --tsv
is a deliberate, reviewable step -- see PLAN.md.
"""
from __future__ import annotations

import argparse
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.path.insert(0, os.path.join(ROOT, "tools"))

from xbuildnames import norm  # noqa: E402  (same normalisation as the demo matcher)

FUNCS_TSV = os.path.join(".shake", "ghidra-export", "functions.tsv")
MAIN = ("disks/tenchu/main.exe", 0x80011000)
MAIN_TEXT_END = 0x80098000

# name -> (image, text vram). run.exe loads high; the rest share main's base.
OTHERS = {
    "trial.exe": ("disks/tenchu/trial.exe", 0x80011000),
    "menu.exe": ("disks/tenchu/menu.exe", 0x80011000),
    "ending.exe": ("disks/tenchu/ending.exe", 0x80011000),
    "run.exe": ("disks/tenchu/run.exe", 0x80110000),
    "slps_019.01": ("disks/slps_019.01", 0x80011000),
}

# A function shorter than this is a `jr $ra` stub or a BIOS thunk: its body occurs
# all over every image and a "hit" means nothing.
MIN_SIZE = 32


def normtext(blob: bytes, off0: int) -> bytes:
    """Normalise a whole image's .text so a normalised body can be `find`-ed in it."""
    return b"".join(
        struct.pack("<I", norm(struct.unpack_from("<I", blob, i)[0]))
        for i in range(off0, len(blob) - 3, 4)
    )


def normbody(b: bytes) -> bytes:
    return normtext(b, 0)


def main_functions(min_size: int = MIN_SIZE) -> list[tuple[str, int, int, bytes]]:
    """(name, vram, size, body) for every main.exe function of at least min_size."""
    img = open(MAIN[0], "rb").read()
    out = []
    for line in open(FUNCS_TSV):
        parts = line.rstrip("\n").split("\t")
        if len(parts) < 3:
            continue
        a, sz, name = int(parts[0], 16), int(parts[1]), parts[2]
        if not (MAIN[1] <= a < MAIN_TEXT_END) or sz < min_size:
            continue
        off = a - MAIN[1] + 0x800
        b = img[off : off + sz]
        if len(b) == sz:
            out.append((name, a, sz, b))
    return out


def matched_set() -> set[str]:
    """The functions we have already byte-matched (same rule as tools/progress.py)."""
    import re

    yaml = open("config/splat.main.exe.yaml").read()
    carved = set(re.findall(r"^\s+- \[0x[0-9A-Fa-f]+,\s*c,\s*(\S+)\]", yaml, re.M))
    out = set()
    src = "src/main.exe"
    for f in os.listdir(src):
        if not f.endswith(".c"):
            continue
        if re.search(r"^\s*INCLUDE_ASM", open(os.path.join(src, f)).read(), re.M):
            continue
        if f[:-2] in carved:
            out.add(f[:-2])
    return out


def find_all(hay: bytes, needle: bytes, limit: int = 3) -> list[int]:
    occ, i = [], hay.find(needle)
    while i != -1 and len(occ) < limit:
        occ.append(i)
        i = hay.find(needle, i + 4)
    return occ


def scan(exe: str, exact: bool, min_size: int):
    path, vram = OTHERS[exe]
    blob = open(path, "rb").read()
    hay = blob[0x800:] if exact else normtext(blob, 0x800)
    funcs = main_functions(min_size)

    hits, ambiguous = [], 0
    for name, addr, sz, body in funcs:
        occ = find_all(hay, body if exact else normbody(body))
        if not occ:
            continue
        if len(occ) > 1:
            ambiguous += 1
            continue
        off = occ[0]
        if off % 4:
            continue
        hits.append((name, addr, vram + off, sz))
    return funcs, hits, ambiguous


def adjacency(hits) -> int:
    """How many hits are immediately followed by another hit, end-to-end.

    Functions keep their source order within a translation unit, so a long run of
    end-to-end hits is independent evidence that the boundaries and addresses are
    right. (vinit/vgetmaxsize/vgetfreesize/vcalloc do exactly this in trial.exe.)
    """
    ends = {h[2] + h[3] for h in hits}
    starts = {h[2] for h in hits}
    return len(ends & starts)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--target", choices=sorted(OTHERS), help="default: all, summary only")
    ap.add_argument("--exact", action="store_true", help="byte-identical bodies only")
    ap.add_argument("--min-size", type=int, default=MIN_SIZE)
    ap.add_argument("--tsv", help="write name<TAB>addr<TAB>size for the hits")
    args = ap.parse_args()

    if not os.path.exists(FUNCS_TSV):
        sys.exit(f"xexe: no {FUNCS_TSV} -- run the Ghidra export first")

    matched = matched_set()
    mode = "exact" if args.exact else "normalized"
    targets = [args.target] if args.target else sorted(OTHERS)

    if not args.target:
        n = len(main_functions(args.min_size))
        print(f"{n} main.exe functions >= {args.min_size} bytes; "
              f"{len(matched)} matched overall; mode={mode}\n")
        print(f"  {'exe':<12} {'hits':>5} {'ambig':>6} {'adjacent':>9} {'ours':>5}")
        print("  " + "-" * 41)

    for exe in targets:
        funcs, hits, ambiguous = scan(exe, args.exact, args.min_size)
        ours = [h for h in hits if h[0] in matched]
        if not args.target:
            print(f"  {exe:<12} {len(hits):>5} {ambiguous:>6} "
                  f"{adjacency(hits):>9} {len(ours):>5}")
            continue

        print(f"{exe}: {len(hits)}/{len(funcs)} main.exe functions found ({mode}), "
              f"{ambiguous} ambiguous, {adjacency(hits)} in end-to-end runs, "
              f"{len(ours)} already matched by us\n")
        for name, addr, there, sz in sorted(hits, key=lambda h: h[2]):
            star = " *" if name in matched else "  "
            print(f"  {there:#010x} {sz:>5}b{star} {name:<34} (main {addr:#010x})")
        print("\n  * = we have byte-matching C for this function already")

        if args.tsv:
            with open(args.tsv, "w") as f:
                for name, addr, there, sz in sorted(hits, key=lambda h: h[2]):
                    f.write(f"{name}\t{there:#010x}\t{sz}\n")
            print(f"\n  wrote {len(hits)} rows to {args.tsv}")


if __name__ == "__main__":
    main()
