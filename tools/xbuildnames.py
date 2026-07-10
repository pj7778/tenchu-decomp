#!/usr/bin/env python3
"""Recover function names by *instruction identity* against the demo's PSX.EXE.

Complements tools/symmatch.py.  symmatch aligns retail against PSX.SYM's debug
info, which only covers the 31 game translation units.  This tool instead
matches on code: for functions that were not edited between the demo and retail,
the instruction stream is identical once link-time immediates are masked out.
That covers exactly what symmatch cannot -- library and support code.

Normalisation, per 32-bit word:
  * R-type / COP0/1/2 (incl. GTE)  kept whole -- all fields are registers
  * j / jal                        opcode only -- absolute target is relocated
  * branches                       kept whole -- PC-relative, survives relinking
  * every other I-type             immediate masked -- %hi/%lo, gp and stack offsets

A name is proposed only when its normalized hash is unique on *both* sides, and
only when the name also appears in PSX.SYM (provenance: the demo executable's
Ghidra names were themselves propagated from PSX.SYM, so this filters out
Ghidra placeholders and boricj's own DEAD_* labels).

Precision is measured, not assumed: pairs where both sides already carry a real
name form a control set, and we report how often the names agree.
"""
from __future__ import annotations

import argparse, collections, hashlib, os, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import psxsym as P

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PLACEHOLDER = ("FUN_", "LAB_", "DAT_", "DEAD_", "thunk", "SUB_", "sub_")


def is_placeholder(n: str) -> bool:
    return n.startswith(PLACEHOLDER)


def norm(w: int) -> int:
    op = w >> 26
    if op == 0 or op in (0x10, 0x11, 0x12):
        return w
    if op in (2, 3):
        return op << 26
    if op == 1 or op in (4, 5, 6, 7):
        return w
    return w & 0xFFFF0000


def toks(data: bytes, off: int, size: int) -> list[int]:
    return [norm(struct.unpack_from("<I", data, off + i)[0]) for i in range(0, size, 4)]


def strict(t: list[int]) -> str:
    return hashlib.sha1(b"".join(struct.pack("<I", x) for x in t)).hexdigest()


def loose(t: list[int]) -> str:
    h = collections.Counter((x >> 26, (x & 0x3F) if (x >> 26) == 0 else -1) for x in t)
    return hashlib.sha1(repr(sorted(h.items())).encode()).hexdigest()


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("sym", nargs="?", default=f"{REPO}/disks/demo/PSX.SYM",
                    help="PSX.SYM (provenance filter)")
    ap.add_argument("--psxexe", default=f"{REPO}/disks/demo/PSX.EXE",
                    help="demo PSX.EXE carved from TENCHU.VOL")
    ap.add_argument("--psxexe-funcs", default=f"{REPO}/reference/demo-psxexe.functions.tsv",
                    help="TSV: addr<TAB>size<TAB>name")
    ap.add_argument("--functions", default=f"{REPO}/.shake/ghidra-export/functions.tsv")
    ap.add_argument("--exe", default=f"{REPO}/disks/tenchu/main.exe")
    ap.add_argument("--apply", metavar="TSV")
    args = ap.parse_args()

    sf = P.load(args.sym)
    known = {f.name for f in sf.funcs} | {s.name for s in sf.syms}

    # --- source: demo PSX.EXE
    exe = open(args.psxexe, "rb").read()
    t_addr, t_size = struct.unpack_from("<II", exe, 0x18)
    text, base = exe[0x800:0x800 + t_size], t_addr
    src: dict[int, tuple[str, list[int]]] = {}
    for line in open(args.psxexe_funcs):
        p = line.rstrip("\n").split("\t")
        if len(p) < 3:
            continue
        a, size, name = int(p[1] if p[0] == "F" else p[0], 16), 0, ""
        if p[0] == "F":
            a, size, name = int(p[1], 16), int(p[2]), p[3]
        else:
            a, size, name = int(p[0], 16), int(p[1]), p[2]
        if size < 8 or size % 4 or not (base <= a and a + size <= base + len(text)):
            continue
        src[a] = (name, toks(text, a - base, size))

    # --- dest: retail main.exe
    rexe = open(args.exe, "rb").read()
    VB, FO = 0x80011000, 0x800
    dst: dict[int, tuple[str, list[int]]] = {}
    for line in open(args.functions):
        p = line.rstrip("\n").split("\t")
        if len(p) < 3:
            continue
        a, size, name = int(p[0], 16), int(p[1]), p[2]
        off = a - VB + FO
        if size < 8 or size % 4 or off < 0 or off + size > len(rexe):
            continue
        dst[a] = (name, toks(rexe, off, size))

    print(f"demo PSX.EXE funcs: {len(src)}   retail funcs: {len(dst)}   "
          f"PSX.SYM names: {len(known)}")

    proposals: dict[int, tuple[str, str, str]] = {}
    for label, fn in (("STRICT", strict), ("LOOSE", loose)):
        sidx, didx = collections.defaultdict(list), collections.defaultdict(list)
        for a, (n, t) in src.items():
            sidx[(fn(t), len(t))].append(n)
        for a, (n, t) in dst.items():
            didx[(fn(t), len(t))].append(a)
        pairs = [(didx[k][0], sidx[k][0]) for k in sidx
                 if len(sidx[k]) == 1 and len(didx.get(k, [])) == 1]

        ctrl = [(a, dst[a][0], n) for a, n in pairs
                if not is_placeholder(dst[a][0]) and not is_placeholder(n)]
        agree = [c for c in ctrl if c[1] == c[2]]
        print(f"\n{label}: unique 1:1 pairs {len(pairs)}   control (both named) {len(ctrl)}   "
              f"agree {len(agree)} ({100*len(agree)/max(1,len(ctrl)):.1f}%)")
        for a, ours, demo in [c for c in ctrl if c[1] != c[2]][:8]:
            print(f"    DISAGREE {a:08x} ours={ours} demo={demo}")

        for a, n in pairs:
            if not is_placeholder(dst[a][0]) or is_placeholder(n) or n not in known:
                continue
            if a not in proposals:                 # STRICT wins over LOOSE
                proposals[a] = (dst[a][0], n, label)

    # a name may not be handed to two addresses, nor duplicate an existing name
    taken = {n for n, _ in dst.values()}
    cnt = collections.Counter(n for _, n, _ in proposals.values())
    proposals = {a: v for a, v in proposals.items()
                 if cnt[v[1]] == 1 and v[1] not in taken}

    print(f"\n=== proposals: {len(proposals)} ===")
    for a in sorted(proposals):
        old, new, how = proposals[a]
        reg = "game" if a < 0x80060000 else "SDK "
        print(f"  {a:08x} [{reg}] {old:24s} -> {new:28s} ({how})")

    if args.apply:
        with open(args.apply, "w") as fh:
            for a in sorted(proposals):
                old, new, how = proposals[a]
                fh.write(f"{a:08x}\t{old}\t{new}\txbuild\t{how}\n")
        print(f"\nwrote {len(proposals)} renames to {args.apply}")


if __name__ == "__main__":
    main()
