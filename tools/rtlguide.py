#!/usr/bin/env python3
"""Turn an assembly residual plus our RTL into a mechanical next action.

The target executable has no RTL.  Its assembly is the specification; our
compiler's RTL is the causal trace.  rtlguide aligns the target and candidate
instructions, classifies each residual by the cc1 pass that owns it, recompiles
the candidate with debug NOTE objects, and maps the mismatching instructions
back to concrete C lines and final hard-register locals.

This replaces the usual manual loop of `asmdiff -> guess a pass -> rtldump ->
grep source lines -> regalloc` with one reproducible report.  The report is also
consumed by `autorules.py --guided`, which restricts its search to the relevant
cookbook transformations and can try byte-neutral enabling edits with a beam.

    tools/rtlguide.py <Name>              build draft + full guided report
    tools/rtlguide.py <Name> --no-build   reuse the current .shake build
    tools/rtlguide.py <Name> --no-rtl     assembly classification only
    tools/rtlguide.py <Name> --json       machine-readable report

Run inside the nix devShell.  The RTL/debug compilation is standalone under
/tmp and never races Shake; only the optional initial ./Build touches .shake.
"""
from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import difflib
import json
import os
import re
import sys

import asmdiff
import matchdiff
import regalloc
import rtldump

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

REG_ORDER = [
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra",
]
REGNUM = {name: i for i, name in enumerate(REG_ORDER)}
REGNUM["s8"] = 30
REG_RE = re.compile(
    r"(?<![\w$])\$?(zero|at|v[01]|a[0-3]|t[0-9]|s[0-8]|k[01]|gp|sp|fp|ra)(?!\w)"
)
NUM_RE = re.compile(r"(?<!\w)-?(?:0x[0-9a-f]+|\d+)(?!\w)", re.I)
BRANCH = {
    "b", "beq", "beqz", "bne", "bnez", "bgez", "bgtz", "blez", "bltz",
    "bgezal", "bltzal", "j", "jal", "jalr", "jr",
}
MOVEISH = {"move", "movn", "movz"}
COMBINE_OPS = {
    "add", "addu", "addiu", "sub", "subu", "and", "andi", "or", "ori",
    "xor", "xori", "nor", "sll", "sllv", "sra", "srav", "srl", "srlv",
    "slt", "slti", "sltu", "sltiu", "mult", "multu", "div", "divu",
    "mfhi", "mflo", "negu", "not",
}

CATEGORY_PASSES = {
    "regalloc": ["lreg", "greg"],
    "cse/coalescing": ["rtl", "cse", "lreg"],
    "jump/cross-jump": ["jump", "jump2"],
    "schedule/delay": ["sched2", "dbr"],
    "combine/expression": ["rtl", "combine"],
    "structure/length": ["rtl", "jump", "jump2"],
}
CATEGORY_RULES = {
    "regalloc": ["copy-barrier", "split-chain", "or-inplace"],
    "cse/coalescing": ["abs-copy-barrier", "copy-barrier"],
    "jump/cross-jump": ["case-fence", "and-nest"],
    "schedule/delay": ["abs-copy-barrier", "cmp-swap", "split-chain", "or-inplace"],
    "combine/expression": [
        "abs-ge", "cmp-swap", "min-ternary", "ptr-index-sum", "split-chain",
    ],
    "structure/length": ["and-nest", "temp-inline", "case-fence"],
}


def mnemonic(insn: str) -> str:
    return insn.strip().split(None, 1)[0] if insn.strip() else ""


def registers(insn: str) -> list[str]:
    return [m.group(1) for m in REG_RE.finditer(insn)]


def shape(insn: str, numbers: bool = False) -> str:
    """Instruction shape used for alignment.

    Register names are deliberately erased: a whole register rotation should
    align as a run of tiny differences, not one giant replace block.  Branch
    destinations are also erased because address drift is not a source-shape
    difference.  `numbers` additionally masks constants for standalone-object
    (unrelocated) -> linked-image alignment.
    """
    s = re.sub(r"\s*<[^>]+>", "", insn.lower())
    s = REG_RE.sub("R", s)
    op = mnemonic(s)
    if op in BRANCH:
        parts = s.rsplit(",", 1)
        if len(parts) == 2:
            s = parts[0] + ",ADDR"
        elif op not in {"jr", "jalr"}:
            bits = s.split(None, 1)
            s = bits[0] + (" ADDR" if len(bits) > 1 else "")
    if numbers:
        s = NUM_RE.sub("#", s)
    return re.sub(r"\s+", " ", s).strip()


def _paired_alignment(target, ours):
    """Yield `(target_index|None, ours_index|None)` in structural order."""
    tkeys = [shape(x[1]) for x in target]
    okeys = [shape(x[1]) for x in ours]
    sm = difflib.SequenceMatcher(None, tkeys, okeys, autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            yield from zip(range(i1, i2), range(j1, j2))
            continue
        common = min(i2 - i1, j2 - j1)
        for k in range(common):
            yield i1 + k, j1 + k
        for i in range(i1 + common, i2):
            yield i, None
        for j in range(j1 + common, j2):
            yield None, j


def _different_pair(t, o) -> bool:
    if t is None or o is None:
        return True
    # Keep branch-target-only drift out of the diagnosis.  It is a consequence
    # of an earlier shape/length difference and asmdiff hides it by default too.
    if mnemonic(t) in BRANCH and shape(t) == shape(o) and registers(t) == registers(o):
        return False
    return t != o


def diff_hunks(target, ours):
    """Group adjacent differing aligned pairs into compact diagnostic hunks."""
    pairs = list(_paired_alignment(target, ours))
    bad = []
    for pos, (ti, oi) in enumerate(pairs):
        ts = target[ti][1] if ti is not None else None
        os_ = ours[oi][1] if oi is not None else None
        if _different_pair(ts, os_):
            bad.append((pos, ti, oi))
    groups = []
    for item in bad:
        if not groups or item[0] > groups[-1][-1][0] + 1:
            groups.append([item])
        else:
            groups[-1].append(item)
    # SequenceMatcher represents `A; branch` -> `branch; A` as an insertion and
    # a deletion separated by the equal branch.  Join only those complementary
    # one-sided groups (not arbitrary neighbouring register hunks), so the
    # scheduler classifier sees the full reordered multiset.
    merged = []
    for group in groups:
        if merged and group[0][0] <= merged[-1][-1][0] + 2:
            left_t = any(x[1] is not None for x in merged[-1])
            left_o = any(x[2] is not None for x in merged[-1])
            right_t = any(x[1] is not None for x in group)
            right_o = any(x[2] is not None for x in group)
            complementary = ((left_t and not left_o and right_o and not right_t)
                             or (left_o and not left_t and right_t and not right_o))
            if complementary:
                start, end = merged[-1][0][0], group[-1][0]
                merged[-1] = [(p, ti, oi) for p, (ti, oi) in
                              enumerate(pairs[start:end + 1], start)]
                continue
        merged.append(group)
    groups = merged
    out = []
    for group in groups:
        tis = [x[1] for x in group if x[1] is not None]
        ois = [x[2] for x in group if x[2] is not None]
        out.append(dict(
            target_indices=tis,
            ours_indices=ois,
            target=[target[i] for i in tis],
            ours=[ours[i] for i in ois],
        ))
    return out, pairs


def classify_hunk(hunk):
    t = [x[1] for x in hunk["target"]]
    o = [x[1] for x in hunk["ours"]]
    tops, oops = [mnemonic(x) for x in t], [mnemonic(x) for x in o]
    same_shape = len(t) == len(o) and all(shape(a) == shape(b) for a, b in zip(t, o))
    if same_shape and any(registers(a) != registers(b) for a, b in zip(t, o)):
        return "regalloc"
    if Counter(tops) == Counter(oops) and tops != oops:
        return "schedule/delay"
    if (sum(x in MOVEISH for x in tops) != sum(x in MOVEISH for x in oops)
            or tops.count("nop") != oops.count("nop")):
        return "cse/coalescing"
    if (len(t) != len(o) and
            (any(x in BRANCH for x in tops + oops) or tops.count("jr") != oops.count("jr"))):
        return "jump/cross-jump"
    if len(t) != len(o):
        return "structure/length"
    if any(x in BRANCH for x in tops + oops):
        return "jump/cross-jump"
    if any(x in COMBINE_OPS for x in tops + oops):
        return "combine/expression"
    return "structure/length"


def _candidate_asm(name):
    addr, size = matchdiff.symbol_slot(name)
    linked = matchdiff.linked_text_size(name)
    target = asmdiff.dis(asmdiff.ORIG, addr, size)
    ours_size = linked if linked is not None else size
    ours = asmdiff.dis(asmdiff.OURS, addr, max(ours_size, 4))
    return addr, size, ours_size, target, ours


def assembly_guide(name):
    """Classify the existing build; fast entry point used by autorules."""
    addr, size, ours_size, target, ours = _candidate_asm(name)
    hunks, pairs = diff_hunks(target, ours)
    counts = Counter()
    clobbers = Counter()
    substitutions = Counter()
    for h in hunks:
        category = classify_hunk(h)
        h["category"] = category
        counts[category] += 1
        t_moves = [x[1] for x in h["target"] if mnemonic(x[1]) == "move"]
        o_moves = [x[1] for x in h["ours"] if mnemonic(x[1]) == "move"]
        if len(t_moves) > len(o_moves):
            for insn in t_moves:
                regs = registers(insn)
                if regs:
                    clobbers[regs[0]] += 1
    for ti, oi in pairs:
        if ti is None or oi is None:
            continue
        t, o = target[ti][1], ours[oi][1]
        if shape(t) != shape(o) or mnemonic(t) != mnemonic(o):
            continue
        tr, or_ = registers(t), registers(o)
        for want, have in zip(tr, or_):
            if want != have:
                substitutions[(have, want)] += 1
    # A target-only move is the strongest clobber signal.  A compact
    # register-only residual is weaker but still worth an opt-in, line-local
    # clobber candidate: it can invalidate the stale hard-register equality
    # that otherwise forces the destination into the candidate register.
    if counts.get("regalloc"):
        for (_have, want), _n in substitutions.most_common(2):
            if want not in {"zero", "sp", "gp", "fp", "s8", "ra"}:
                clobbers[want] += 1
    if ours_size != size:
        counts["structure/length"] += 1
    primary = counts.most_common(1)[0][0] if counts else "matched"
    rules = []
    for category, _ in counts.most_common():
        for rule in CATEGORY_RULES.get(category, []):
            if rule not in rules:
                rules.append(rule)
    if clobbers and "hard-clobber" not in rules:
        rules.append("hard-clobber")
    return dict(
        name=name, address=addr, target_bytes=size, ours_bytes=ours_size,
        target_instructions=len(target), ours_instructions=len(ours),
        primary=primary, categories=dict(counts), rules=rules,
        passes=CATEGORY_PASSES.get(primary, []),
        hard_clobbers=[r for r, _ in clobbers.most_common(2)],
        register_substitutions=[
            dict(ours=a, target=b, count=n)
            for (a, b), n in substitutions.most_common()
        ],
        hunks=hunks, target=target, ours=ours,
    )


def parse_line_object(path, name):
    """Instructions from `objdump -drSl`, each carrying its current C line."""
    out = []
    current_line = None
    in_func = False
    label = re.compile(r"^[0-9a-f]+\s+<([^>]+)>:$", re.I)
    src_line = re.compile(r"\.c:(\d+)(?:\s|$)")
    insn = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(.+)$", re.I)
    for raw in open(path, errors="replace"):
        s = raw.rstrip("\n")
        m = label.match(s)
        if m:
            # objdump prints every internal LM/$L label in the same `<...>:`
            # form as a function symbol.  Once the requested function starts,
            # keep consuming; structural alignment naturally ignores any later
            # helper function in the rare multi-function source file.
            if m.group(1) == name:
                in_func = True
                current_line = None
            continue
        if not in_func:
            continue
        m = src_line.search(s)
        if m:
            current_line = int(m.group(1))
            continue
        m = insn.match(s)
        if m:
            text = re.sub(r"\s+", " ", m.group(3).replace("\t", " ")).strip()
            out.append(dict(offset=int(m.group(1), 16), asm=text, line=current_line))
    return out


def map_source_lines(line_insns, ours):
    """Map linked candidate instruction indices to C lines via standalone .o."""
    a = [shape(x["asm"], numbers=True) for x in line_insns]
    b = [shape(x[1], numbers=True) for x in ours]
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    mapped = {}
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for i, j in zip(range(i1, i2), range(j1, j2)):
                mapped[j] = line_insns[i]["line"]
        elif tag == "replace":
            # A register/immediate normalization corner can still leave an
            # equal-length replacement.  Positional mapping is more useful
            # than dropping every line in that small region.
            for i, j in zip(range(i1, i2), range(j1, j2)):
                mapped[j] = line_insns[i]["line"]
    return mapped


def parse_debug_variables(path, name):
    """Final hard-register homes emitted by cc1's stabs `.def` records."""
    by_reg = defaultdict(list)
    in_func = False
    definition = re.compile(
        r"\.def\s+([A-Za-z_]\w*);\s*\.val\s+(\d+);\s*\.scl\s+(4|17);"
    )
    for raw in open(path, errors="replace"):
        s = raw.strip()
        if s == name + ":":
            in_func = True
            continue
        if in_func and re.search(rf"\.end\s+{re.escape(name)}\b", s):
            break
        if not in_func:
            continue
        m = definition.search(s)
        if not m:
            continue
        num = int(m.group(2))
        if num in regalloc.REGNAME and m.group(1) not in by_reg[regalloc.rn(num)]:
            by_reg[regalloc.rn(num)].append(m.group(1))
    return dict(by_reg)


def _dump_suffix(path):
    return path.rsplit(".", 1)[-1]


def pass_stats(paths):
    stats = {}
    for path in paths:
        suffix = _dump_suffix(path)
        text = open(path, errors="replace").read()
        stats[suffix] = dict(
            insns=len(re.findall(r"^\((?:insn|jump_insn|call_insn)\b", text, re.M)),
            labels=len(re.findall(r"^\(code_label\b", text, re.M)),
            moves=len(re.findall(r"\{mov\w*", text)),
            source_notes=len(re.findall(r'^\(note .*\(".*\.c"\) \d+\)', text, re.M)),
        )
    return stats


def _attach_lines(guide, mapped):
    all_lines = set()
    for h in guide["hunks"]:
        lines = {mapped[i] for i in h["ours_indices"] if mapped.get(i) is not None}
        if not lines and h["ours_indices"]:
            # Nearest mapped neighbour gives target-only/deleted instructions a
            # useful insertion site rather than no source location at all.
            pivot = h["ours_indices"][0]
            neighbours = sorted(mapped, key=lambda x: abs(x - pivot))
            if neighbours and mapped[neighbours[0]] is not None:
                lines.add(mapped[neighbours[0]])
        h["source_lines"] = sorted(lines)
        all_lines.update(lines)
    guide["source_lines"] = sorted(all_lines)


def _byte_counts(addr, size):
    off = matchdiff.FILE_TEXT_OFF + (addr - matchdiff.TEXT_START)
    orig = open(matchdiff.ORIG, "rb").read()
    ours = open(matchdiff.OURS, "rb").read()
    local = sum(a != b for a, b in zip(orig[off:off + size], ours[off:off + size]))
    total = sum(a != b for a, b in zip(orig, ours)) + abs(len(orig) - len(ours))
    return local, total


def build_candidate(name):
    path = os.path.join("src", "main.exe", name + ".c")
    env = dict(os.environ)
    if os.path.exists(path) and "ifndef NON_MATCHING" in open(path).read():
        env["NON_MATCHING"] = name
    r = matchdiff.run_build(env)
    if r.returncode:
        raise RuntimeError(f"./Build failed (rc={r.returncode}):\n{matchdiff.build_log_tail()}")


def diagnose(name, build=True, rtl=True):
    if build:
        build_candidate(name)
    guide = assembly_guide(name)
    local, total = _byte_counts(guide["address"], guide["target_bytes"])
    guide.update(local_differing_bytes=local, whole_image_differing_bytes=total)
    if not rtl or guide["primary"] == "matched":
        guide["source_lines"] = []
        guide["variables_by_register"] = {}
        guide["pass_stats"] = {}
        return guide

    src = os.path.join("src", "main.exe", name + ".c")
    draft = os.path.exists(src) and "ifndef NON_MATCHING" in open(src).read()
    result = rtldump.compile_rtl(name, ["all"], draft=draft,
                                 debug_lines=True, assemble=True)
    line_insns = parse_line_object(result["objdump"], name)
    mapped = map_source_lines(line_insns, guide["ours"])
    _attach_lines(guide, mapped)
    guide["variables_by_register"] = parse_debug_variables(result["asm"], name)
    guide["pass_stats"] = pass_stats(result["dumps"])

    greg_path = next((p for p in result["dumps"] if p.endswith(".greg")), None)
    alloc = None
    if greg_path:
        funcs = regalloc.split_functions(open(greg_path).read())
        if name in funcs:
            alloc = regalloc.analyse(name, funcs[name])
    for sub in guide["register_substitutions"]:
        sub["variables"] = guide["variables_by_register"].get(sub["ours"], [])
        if alloc and sub["ours"] in REGNUM:
            hard = REGNUM[sub["ours"]]
            pseudos = []
            for pseudo, home in sorted(alloc["disp"].items()):
                if home == hard:
                    pseudos.append(dict(
                        pseudo=pseudo,
                        preferences=[regalloc.rn(x) for x in alloc["preferences"].get(pseudo, [])],
                    ))
            sub["pseudos"] = pseudos
    return guide


def _jsonable(guide):
    def clean(value):
        if isinstance(value, dict):
            return {k: clean(v) for k, v in value.items()}
        if isinstance(value, tuple):
            return list(value)
        if isinstance(value, list):
            return [clean(x) for x in value]
        return value
    result = dict(guide)
    # The top-level streams duplicate every instruction and are only an
    # internal alignment aid.  Keep each hunk's compact target/ours snippets.
    result.pop("target", None)
    result.pop("ours", None)
    result["hunks"] = [
        {k: v for k, v in h.items()
         if k not in {"target_indices", "ours_indices"}}
        for h in guide.get("hunks", [])
    ]
    return clean(result)


def print_report(g, max_hunks=12):
    print(f"rtlguide {g['name']} @ {g['address']:#x}: "
          f"{g['local_differing_bytes']} differing bytes "
          f"({g['whole_image_differing_bytes']} whole image), "
          f"length {g['ours_bytes']} / {g['target_bytes']}")
    if g["primary"] == "matched":
        print("  MATCH — no residual to diagnose.")
        return
    cats = ", ".join(f"{k}={v}" for k, v in
                     sorted(g["categories"].items(), key=lambda x: (-x[1], x[0])))
    print(f"  owner: {g['primary']}  [{cats}]")
    print(f"  inspect: {', '.join('.' + x for x in g['passes'])}")

    for n, h in enumerate(g["hunks"][:max_hunks], 1):
        line = ",".join(map(str, h.get("source_lines", []))) or "?"
        print(f"\n  hunk {n}: {h['category']}  C line {line}")
        for addr, insn in h["target"][:6]:
            print(f"    T {addr:#x}  {insn}")
        for addr, insn in h["ours"][:6]:
            print(f"    O {addr:#x}  {insn}")
        if len(h["target"]) > 6 or len(h["ours"]) > 6:
            print("    ...")
    if len(g["hunks"]) > max_hunks:
        print(f"\n  ... {len(g['hunks']) - max_hunks} more hunks (use --max-hunks)")

    if g["register_substitutions"]:
        print("\n  register goals (candidate -> target):")
        for sub in g["register_substitutions"][:12]:
            vars_ = ",".join(sub.get("variables", [])) or "?"
            prefs = sorted({p for x in sub.get("pseudos", []) for p in x["preferences"]})
            ptext = ("; allocator preferences " + ",".join(prefs)) if prefs else ""
            print(f"    ${sub['ours']} -> ${sub['target']} x{sub['count']}: "
                  f"locals {vars_}{ptext}")

    interesting = ["rtl", "cse", "combine", "lreg", "greg", "sched2", "jump2", "dbr"]
    present = [x for x in interesting if x in g.get("pass_stats", {})]
    if present:
        print("\n  RTL pass trace (top-level insns / labels / moves):")
        print("    " + "  ".join(
            f"{p}={g['pass_stats'][p]['insns']}/{g['pass_stats'][p]['labels']}/"
            f"{g['pass_stats'][p]['moves']}" for p in present))

    print("\n  mechanical search:")
    print(f"    rules: {', '.join(g['rules']) or '(no local rule; source structure first)'}")
    if g["hard_clobbers"]:
        print("    inferred missing-move clobbers: " +
              ", ".join("$" + x for x in g["hard_clobbers"]))
    print(f"    tools/autorules.py {g['name']} --guided")
    print("  Exact bytes remain authoritative; review any partial-score improvement.")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("name")
    ap.add_argument("--no-build", action="store_true", help="reuse current .shake build")
    ap.add_argument("--no-rtl", action="store_true", help="skip standalone dumps/line map")
    ap.add_argument("--json", action="store_true", help="machine-readable report")
    ap.add_argument("--max-hunks", type=int, default=12)
    args = ap.parse_args()
    try:
        guide = diagnose(args.name, build=not args.no_build, rtl=not args.no_rtl)
    except (FileNotFoundError, ValueError, RuntimeError) as e:
        sys.exit(f"rtlguide: {e}")
    if args.json:
        print(json.dumps(_jsonable(guide), indent=2, sort_keys=True))
    else:
        print_report(guide, args.max_hunks)
    return 0 if guide["primary"] == "matched" else 1


if __name__ == "__main__":
    sys.exit(main())
