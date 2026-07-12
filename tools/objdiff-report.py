#!/usr/bin/env python3
"""Emit an objdiff v2 progress `Report` (JSON) that decomp.dev ingests.

decomp.dev is *artifact-driven*: it never reads decomp.yaml. Its bot scans a
GitHub repo's Actions runs, downloads an artifact named `<version>_report`
(here `jp_report`), reads `report.json` from it, parses it as an objdiff
`Report` (protobuf schema, JSON-encoded), and stores per-commit progress. See
docs/decomp-dev.md for the full pipeline + self-hosting setup.

This generator is *build-free*: like tools/progress.py it derives everything
from static repo files (the function inventory + the splat/symbol config + the
matched `.c` files), so CI needs only Python — no nix, no cc1, no ELF build.

  tools/objdiff-report.py                      -> .shake/build/tenchu/report.json
  tools/objdiff-report.py --out report.json    (CI: artifact payload)
  tools/objdiff-report.py --include-sdk         include the PsyQ SDK block too
  tools/objdiff-report.py --stdout              print JSON to stdout

"Matched" is defined exactly as in tools/progress.py: a function that is CARVED
(has a `c` subsegment in the splat yaml, so its `.c` is actually linked) AND
whose src/main.exe/<Name>.c has no INCLUDE_ASM. Each function becomes one
objdiff *unit* (per-function granularity); units are tagged into the `game` /
`sdk` progress categories. u64 measure/size/address fields are JSON strings and
u32/float fields are numbers — the encoding objdiff's prost+serde emits and
`Report::parse` expects (verified against a live decomp.dev report).
"""
import argparse
import json
import os
import re
import shutil
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

# Kept in sync with tools/progress.py — the game/SDK boundary and text window.
TEXT_START = 0x80011000
TEXT_END = 0x80098000
SDK_START = 0x80060000  # 0x80060xxx+ is the differently-compiled PsyQ SDK block

# The Ghidra export (addr<TAB>size<TAB>name) is gitignored; the committed
# snapshot is the CI / fresh-checkout fallback. When the live export is present
# it wins AND is copied to the snapshot so a local `./Build report` keeps the
# committed inventory current (commit the diff alongside the report change).
LIVE_TSV = ".shake/ghidra-export/functions.tsv"
SNAPSHOT_TSV = "config/functions.main.exe.tsv"
SYMBOLS = "config/symbols.main.exe.txt"
YAML = "config/splat.main.exe.yaml"
SRC = "src/main.exe"
DEFAULT_OUT = ".shake/build/tenchu/report.json"

REPORT_VERSION = 2

CATEGORIES = [
    ("game", "Game code"),
    ("sdk", "PsyQ SDK (LIBCD/LIBSPU/CRT...)"),
]


def resolve_inventory(explicit):
    """Pick the function-inventory TSV; refresh the committed snapshot from the
    live Ghidra export when that is what we read."""
    if explicit:
        return explicit
    if os.path.exists(LIVE_TSV):
        # Keep the committed snapshot in lockstep with the local export.
        if not os.path.exists(SNAPSHOT_TSV) or (
            open(LIVE_TSV, "rb").read() != open(SNAPSHOT_TSV, "rb").read()
        ):
            os.makedirs(os.path.dirname(SNAPSHOT_TSV), exist_ok=True)
            shutil.copyfile(LIVE_TSV, SNAPSHOT_TSV)
            print(f"note: refreshed {SNAPSHOT_TSV} from {LIVE_TSV} "
                  "(commit it)", file=sys.stderr)
        return LIVE_TSV
    if os.path.exists(SNAPSHOT_TSV):
        return SNAPSHOT_TSV
    sys.exit(f"error: no function inventory found ({LIVE_TSV} or {SNAPSHOT_TSV})")


def load_functions(tsv):
    """[(addr, size, name)] for real, named functions inside the text window."""
    funcs = []
    for line in open(tsv):
        p = line.rstrip("\n").split("\t")
        if len(p) == 3 and re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", p[2]):
            a, s, n = int(p[0], 16), int(p[1]), p[2]
            if TEXT_START <= a < TEXT_END:
                funcs.append((a, s, n))
    funcs.sort()
    return funcs


def load_matched():
    """Return (matched_addrs, addr_to_cfile) using progress.py's exact rule.

    A `.c` counts only if it is carved (linked) and contains no INCLUDE_ASM;
    its address comes from the symbol table (Ghidra's inventory name for the
    same function may differ from the `.c` filename, e.g. FUN_... vs a real
    name), so matching is done by address, consistent with tools/progress.py.
    """
    sym_addr = {}
    for line in open(SYMBOLS):
        m = re.match(r"([A-Za-z_$][\w$]*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", line)
        if m:
            sym_addr[m.group(1)] = int(m.group(2), 16)
    carved = set(re.findall(r"^\s+- \[0x[0-9A-Fa-f]+,\s*c,\s*(\S+)\]",
                            open(YAML).read(), re.M))
    matched_addrs = set()
    addr_to_cfile = {}
    for f in sorted(os.listdir(SRC)):
        if not f.endswith(".c"):
            continue
        name = f[:-2]
        if name not in carved:
            continue
        if re.search(r"^\s*INCLUDE_ASM", open(os.path.join(SRC, f)).read(), re.M):
            continue
        if name in sym_addr:
            a = sym_addr[name]
            matched_addrs.add(a)
            addr_to_cfile[a] = f"{SRC}/{f}"
    return matched_addrs, addr_to_cfile


def measures(total_code, matched_code, total_funcs, matched_funcs,
             total_units, complete_units):
    """An objdiff `Measures`, JSON-encoded like prost+serde: u64 -> string,
    u32 -> number, float -> number; default (0) fields omitted. `complete_*`
    tracks fully-matched code, which for us equals `matched_*` (a matched
    function reassembles byte-identically — there are no partial percentages)."""
    def pct(num, den):
        return round(100.0 * num / den, 6) if den else 0.0

    code_pct = pct(matched_code, total_code)
    fn_pct = pct(matched_funcs, total_funcs)
    m = {}
    # fuzzy == matched for us (no sub-function partial credit).
    if total_code:
        m["fuzzy_match_percent"] = code_pct
        m["total_code"] = str(total_code)
    if matched_code:
        m["matched_code"] = str(matched_code)
        m["matched_code_percent"] = code_pct
        m["complete_code"] = str(matched_code)
        m["complete_code_percent"] = code_pct
    if total_funcs:
        m["total_functions"] = total_funcs
    if matched_funcs:
        m["matched_functions"] = matched_funcs
        m["matched_functions_percent"] = fn_pct
    if total_units:
        m["total_units"] = total_units
    if complete_units:
        m["complete_units"] = complete_units
    return m


def build_report(funcs, matched_addrs, addr_to_cfile, include_sdk):
    units = []
    # category id -> accumulator [tc, mc, tf, mf, tu, cu]
    acc = {cid: [0, 0, 0, 0, 0, 0] for cid, _ in CATEGORIES}
    tot = [0, 0, 0, 0, 0, 0]

    for addr, size, name in funcs:
        is_sdk = addr >= SDK_START
        if is_sdk and not include_sdk:
            continue
        cat = "sdk" if is_sdk else "game"
        matched = addr in matched_addrs
        cu = 1 if matched else 0
        mc = size if matched else 0
        mf = 1 if matched else 0

        for bucket in (acc[cat], tot):
            bucket[0] += size
            bucket[1] += mc
            bucket[2] += 1
            bucket[3] += mf
            bucket[4] += 1
            bucket[5] += cu

        func_item = {
            "name": name,
            "size": str(size),
            "fuzzy_match_percent": 100.0 if matched else 0.0,
            "address": str(addr),
        }
        meta = {
            "complete": matched,
            "progress_categories": [cat],
            "auto_generated": False,
        }
        if matched and addr in addr_to_cfile:
            meta["source_path"] = addr_to_cfile[addr]
        units.append({
            "name": name,
            "measures": measures(size, mc, 1, mf, 1, cu),
            "sections": [],
            "functions": [func_item],
            "metadata": meta,
        })

    categories = []
    for cid, cname in CATEGORIES:
        a = acc[cid]
        if a[2] == 0:  # no units in this category (e.g. sdk omitted)
            continue
        categories.append({"id": cid, "name": cname, "measures": measures(*a)})

    return {
        "measures": measures(*tot),
        "units": units,
        "version": REPORT_VERSION,
        "categories": categories,
    }


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", default=DEFAULT_OUT,
                    help=f"output path (default {DEFAULT_OUT})")
    ap.add_argument("--stdout", action="store_true", help="write JSON to stdout")
    ap.add_argument("--include-sdk", action="store_true",
                    help="also include the PsyQ SDK block (top-level = whole image)")
    ap.add_argument("--functions", help="function-inventory TSV (overrides autodetect)")
    args = ap.parse_args()

    tsv = resolve_inventory(args.functions)
    funcs = load_functions(tsv)
    matched_addrs, addr_to_cfile = load_matched()
    report = build_report(funcs, matched_addrs, addr_to_cfile, args.include_sdk)

    text = json.dumps(report, indent=1)
    if args.stdout:
        print(text)
    else:
        os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
        with open(args.out, "w") as fh:
            fh.write(text)

    tm = report["measures"]
    tc, mc = int(tm.get("total_code", 0)), int(tm.get("matched_code", 0))
    pct = 100.0 * mc / tc if tc else 0.0
    scope = "game+sdk" if args.include_sdk else "game"
    print(f"report ({scope}): {len(report['units'])} units, "
          f"{tm.get('matched_functions', 0)}/{tm.get('total_functions', 0)} functions, "
          f"{mc}/{tc} bytes ({pct:.2f}%) -> "
          f"{'stdout' if args.stdout else args.out}", file=sys.stderr)


if __name__ == "__main__":
    main()
