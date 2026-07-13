#!/usr/bin/env python3
"""Recover names by *call signature* against the demo's PSX.EXE.

The third matcher, and the only one that survives a rewritten body.

tools/xbuildnames.py needs the instruction stream to be identical; tools/
symmatch.py needs the function to sit in a debug-info TU.  A function that was
edited between the demo and retail, and lives outside the 31 debug TUs, is
invisible to both.  But *who it calls* is a strong fingerprint that edits rarely
destroy: `jal` targets resolve to names we already know on both sides.

For each function we build the sorted multiset of its named callees.  A
signature with at least MIN_CALLS distinct named callees, unique on both sides,
identifies the function.  Rare-callee weighting is unnecessary: uniqueness on
both sides already does the work.

Precision is measured against the control set (functions already named on both
sides), exactly as in xbuildnames.py.
"""
from __future__ import annotations

import argparse, collections, math, os, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import psxsym as P
import function_inventory as FI

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PLACEHOLDER = ("FUN_", "LAB_", "DAT_", "DEAD_", "thunk", "SUB_", "sub_")
MIN_CALLS = 3


def is_placeholder(n: str) -> bool:
    return n.startswith(PLACEHOLDER)


def callees(data: bytes, base: int, addr: int, size: int, names: dict[int, str]) -> tuple:
    """Sorted multiset of named `jal` targets inside [addr, addr+size)."""
    off = addr - base
    out = []
    for i in range(0, size, 4):
        w = struct.unpack_from("<I", data, off + i)[0]
        if w >> 26 == 3:                                    # jal
            tgt = (addr & 0xF0000000) | ((w & 0x03FFFFFF) << 2)
            n = names.get(tgt)
            if n and not is_placeholder(n):
                out.append(n)
    return tuple(sorted(out))


def load_side(text: bytes, base: int, funcs: list[tuple[int, int, str]]):
    names = {a: n for a, _, n in funcs}
    sig = {}
    for a, s, n in funcs:
        if s < 8 or a - base < 0 or a - base + s > len(text):
            continue
        c = callees(text, base, a, s, names)
        if len(set(c)) >= MIN_CALLS:
            sig[a] = (n, c)
    return sig


def ambiguity_candidates(addr, rn, retail_calls, demo_calls, demo_size):
    """Retail functions that Pareto-dominate a proposed full-containment fit.

    Containment alone can collide for sibling callbacks.  An alternative is a
    conservative blocker only when it also contains every demo call, has no
    more extra named calls, and is at least as close in code size.  This catches
    the historical AttackFire misallocation without flagging any other entry in
    the applied-name control table.
    """
    if addr not in retail_calls or not demo_calls or addr not in rn:
        return []
    proposed = retail_calls[addr]
    extra = sum((proposed - demo_calls).values())
    size_gap = abs(math.log(max(1, rn[addr][0]) / max(1, demo_size)))
    out = []
    total = sum(demo_calls.values())
    for other, calls in retail_calls.items():
        if other == addr or other not in rn:
            continue
        cover = sum((calls & demo_calls).values()) / total
        other_extra = sum((calls - demo_calls).values())
        other_gap = abs(math.log(max(1, rn[other][0]) / max(1, demo_size)))
        if cover == 1.0 and other_extra <= extra and other_gap <= size_gap:
            out.append((other_extra, other_gap, other, rn[other][0], rn[other][1]))
    return sorted(out)


def verify(rexe, rfuncs, text, t_addr, dfuncs, tsv: str) -> int:
    """Re-check a proposed rename table: does the retail function call what the
    demo function of that name calls?  Containment (demo callees ⊆ retail) is the
    criterion, not equality -- retail functions grow.  Returns the reject count.

    This caught a real error: symmatch proposed SetPadState for 0x80032610 on
    matching frame size + saved-reg mask, but the callees said UpdateTexScroll.
    """
    rn = {a: (s, n) for a, s, n in rfuncs}
    dn = {a: (s, n) for a, s, n in dfuncs}
    byname = {n: a for a, (_, n) in dn.items()}
    rejects = 0
    rnames = {k: v[1] for k, v in rn.items()}
    rtext, rbase = rexe[0x800:], 0x80011000
    retail_calls = {}
    for a, (size, _) in rn.items():
        off = a - rbase
        if size >= 8 and off >= 0 and off + size <= len(rtext):
            retail_calls[a] = collections.Counter(
                callees(rtext, rbase, a, size, rnames)
            )
    print(f"{'addr':9s} {'proposed':28s} {'verdict':9s} evidence")
    for line in open(tsv):
        p = line.rstrip("\n").split("\t")
        if len(p) < 3 or line.startswith("#"):
            continue
        a, new = int(p[0], 16), p[2]
        if a not in rn or new not in byname:
            print(f"{a:08x} {new:28s} {'n/a':9s} (absent from one side)")
            continue
        rc = retail_calls.get(a, collections.Counter())
        da = byname[new]
        dc = collections.Counter(callees(text, t_addr, da, dn[da][0],
                                         {k: v[1] for k, v in dn.items()}))
        if not dc and not rc:
            print(f"{a:08x} {new:28s} {'leaf':9s} (no named calls either side)")
            continue
        cover = sum((rc & dc).values()) / max(1, sum(dc.values()))
        collision = []
        if cover == 1.0 and sum(dc.values()) >= 2:
            collision = ambiguity_candidates(a, rn, retail_calls, dc, dn[da][0])
            if collision:
                v = "AMBIGUOUS"; rejects += 1
            else:
                v = "CONFIRM"
        elif cover >= 0.5:
            v = "WEAK"
        else:
            v = "REJECT"; rejects += 1
        miss = sorted((dc - rc).elements())
        evidence = f"demo-callees covered {cover:.0%}"
        if miss:
            evidence += f", missing {miss[:4]}"
        if collision:
            ex, _, ca, cs, cn = collision[0]
            evidence += (f", competing fit {ca:08x} {cn}"
                         f" (extra calls {ex}, size {cs}/{dn[da][0]})")
        print(f"{a:08x} {new:28s} {v:9s} {evidence}")
    return rejects


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("sym", nargs="?", default=f"{REPO}/disks/demo/PSX.SYM")
    ap.add_argument("--psxexe", default=f"{REPO}/disks/demo/PSX.EXE")
    ap.add_argument("--psxexe-funcs", default=f"{REPO}/reference/demo-psxexe.functions.tsv")
    ap.add_argument("--functions", default=f"{REPO}/.shake/ghidra-export/functions.tsv")
    ap.add_argument("--splat", default=f"{REPO}/config/splat.main.exe.yaml",
                    help="current named C subsegments overlaid onto the function inventory")
    ap.add_argument("--no-name-overlay", action="store_true",
                    help="use names in --functions verbatim (normally stale after renames)")
    ap.add_argument("--exe", default=f"{REPO}/disks/tenchu/main.exe")
    ap.add_argument("--apply", metavar="TSV")
    ap.add_argument("--verify", metavar="TSV",
                    help="re-check an existing rename table instead of proposing")
    args = ap.parse_args()

    known = set()
    sf = P.load(args.sym)
    known = {f.name for f in sf.funcs} | {s.name for s in sf.syms}

    exe = open(args.psxexe, "rb").read()
    t_addr, t_size = struct.unpack_from("<II", exe, 0x18)
    dfuncs = []
    for line in open(args.psxexe_funcs):
        p = line.rstrip("\n").split("\t")
        if len(p) >= 3:
            dfuncs.append((int(p[0], 16), int(p[1]), p[2]))
    dsig = load_side(exe[0x800:0x800 + t_size], t_addr, dfuncs)

    rexe = open(args.exe, "rb").read()
    rfuncs = FI.load_functions(args.functions)
    if not args.no_name_overlay:
        rfuncs, renamed = FI.overlay_current_names(rfuncs, args.splat)
        print(f"retail current-name overlays: {renamed}")
    if args.verify:
        n = verify(rexe, rfuncs, exe[0x800:0x800 + t_size], t_addr, dfuncs, args.verify)
        print(f"\n{n} rejected/ambiguous" if n else "\nno rejects or ambiguities")
        sys.exit(1 if n else 0)

    # main.exe text is addressed from 0x80011000 at file offset 0x800
    rsig = load_side(rexe[0x800:], 0x80011000, rfuncs)

    print(f"demo funcs with >={MIN_CALLS} named callees: {len(dsig)}")
    print(f"retail funcs with >={MIN_CALLS} named callees: {len(rsig)}")

    didx, ridx = collections.defaultdict(list), collections.defaultdict(list)
    for a, (n, c) in dsig.items():
        didx[c].append(n)
    for a, (n, c) in rsig.items():
        ridx[c].append(a)

    pairs = [(ridx[c][0], didx[c][0]) for c in didx
             if len(didx[c]) == 1 and len(ridx.get(c, [])) == 1]

    ctrl = [(a, rsig[a][0], n) for a, n in pairs
            if not is_placeholder(rsig[a][0]) and not is_placeholder(n)]
    agree = [c for c in ctrl if c[1] == c[2]]
    print(f"\nunique 1:1 call-signature pairs: {len(pairs)}")
    print(f"control (both named): {len(ctrl)}   agree: {len(agree)} "
          f"({100*len(agree)/max(1,len(ctrl)):.1f}%)")
    for a, ours, demo in [c for c in ctrl if c[1] != c[2]][:10]:
        print(f"    DISAGREE {a:08x} ours={ours} demo={demo}")

    taken = {n for _, _, n in rfuncs}
    props = {}
    for a, n in pairs:
        if not is_placeholder(rsig[a][0]) or is_placeholder(n) or n not in known or n in taken:
            continue
        props[a] = (rsig[a][0], n, len(rsig[a][1]))
    cnt = collections.Counter(v[1] for v in props.values())
    props = {a: v for a, v in props.items() if cnt[v[1]] == 1}

    print(f"\n=== proposals: {len(props)} ===")
    for a in sorted(props):
        old, new, ncalls = props[a]
        print(f"  {a:08x} {old:24s} -> {new:28s} ({ncalls} calls)")

    if args.apply:
        with open(args.apply, "w") as fh:
            for a in sorted(props):
                old, new, ncalls = props[a]
                fh.write(f"{a:08x}\t{old}\t{new}\tcallmatch\tCALLS{ncalls}\n")
        print(f"\nwrote {len(props)} renames to {args.apply}")


if __name__ == "__main__":
    main()
