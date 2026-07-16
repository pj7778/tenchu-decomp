# The restricted GTE inline-asm policy

Adopted 2026-07-16 by the project owner. This resolves the long-standing
"inline-asm policy" question that parked the GTE/COP2 class (previously
~22% of the remaining unmatched game bytes).

## The decision

Inline assembly is allowed in matched source **only** through the shared macro
header [`src/main.exe/gte.h`](../src/main.exe/gte.h), and **only** in functions
listed in [`config/gte-allowlist.txt`](../config/gte-allowlist.txt). Everywhere
else, `__asm__` in a draft remains an automatic failure
(`tools/matchdiff.py` `source_completion_blockers`), exactly as before.

Rationale: cc1 2.8.1 has **no C construct that emits a COP2 opcode**, and the
`draw*` render handlers additionally **receive their arguments in
`$t2..$t5`/`$s0`**, outside the ABI. The original developers did not write
these in C either — PsyQ's own `libgte` interface (`INLINE_N.H`/`inline_c.h`)
is a layer of inline-asm macros (`gte_ldDQA()`, `gte_rtps()`, …). A restricted
macro header reconstructing that API **is** the faithful decompilation of the
original source, the same way N64 decomps accept GBI macros. It is not a
matching shortcut.

## Rules

1. `__asm__` may appear in exactly ONE file under `src/`: `gte.h`. A unit test
   (`tools/tests/test_matching_tools.py`) enforces this containment, and that
   every `.c` including `gte.h` is on the allowlist.
2. `gte.h` contains only:
   - COP2 data moves (`mtc2`/`mfc2`/`ctc2`/`cfc2`/`lwc2`/`swc2`) behind
     PsyQ-named macros (`gte_ldDQA`, `gte_ldv0`, `gte_stsxy`, …);
   - GTE command issues as `.word 0x4A......` with the mnemonic in a comment
     (`gte_rtps`, `gte_nclip`, …). Use `.word`: the `.c` pipeline's `as` input
     does not include splat's `gte_macros.inc`, and `.word` is
     assembler-agnostic. NOTE splat's `/* addr vaddr WORD */` comment column in
     carved `.s` is the little-endian FILE bytes, not the instruction word —
     `RTPT` prints `3000284A` and encodes as `.word 0x4A280030`.
   - Nothing with side effects a C statement could express instead.
3. Macros take/return values through `"r"`/`"=r"` constraints. Where a
   function's entry convention is non-ABI (the `DrawTMD` handler family),
   register-pinned locals are allowed in the whitelisted `.c` itself:
   `register long p __asm__("$12");` (verified working in this cc1 — see the
   PClseek.c investigation header).
4. New macros: mirror the real `INLINE_N.H` name if one exists; otherwise use
   the PsyQ naming style. Matcher agents may extend `gte.h` when their target
   needs a missing operation, and must report the addition for review.
5. Additions to the whitelist are an owner-level decision. In particular, the
   `GetPad`/`GetPadXY`/`FUN_8001b174` sign-extension trio and `PClseek` stay
   OFF the list deliberately: their originals were almost certainly plain C
   (a different compiler fold / a libsn assembly object), so asm bodies there
   would be unfaithful. They remain parked NON_MATCHING.

## Verified pipeline facts (the SetDepthQ spike, 2026-07-16)

- cc1 2.8.1 passes extended-asm blocks through as `#APP`/`#NO_APP` regions;
  operands with `"r"` constraints allocate normally (a0/a1 arrived unmoved).
- maspsx treats the `#APP`/`#NO_APP` markers as non-instructions and passes
  unknown COP2 mnemonics through untouched (its transforms key on specific
  load/store/li/la/div opcodes).
- GNU `as` (binutils 2.40, `-march=r3000`) assembles the COP2 data moves
  natively; only GTE *command* mnemonics need the `.word` form in `gte.h`.
- Result: `SetDepthQ` (2 × `ctc2`) matched byte-exactly on the first build,
  whole image identical.
- Volatile asm blocks are scheduling barriers for cc1 — faithful, since the
  original macros were equally opaque to the original compiler.

## The whitelist (initial, 25 functions)

`config/gte-allowlist.txt` is the machine-readable copy; it starts as:
`SetDepthQ` (matched), `DrawTMD`, the 16 primitive renderers
`drawF3 drawG3 drawzF3 drawF4 drawFT3 drawzG3 drawG4 drawGT3 drawzFT3
drawzF4 drawFT4 drawzGT3 drawzG4 drawGT4 drawzFT4 drawzGT4`, the twin pairs
`FUN_80058c70/FUN_80059008`, `FUN_80059ff4/FUN_8005a3cc`,
`FUN_8005961c/FUN_80059b08`, and `FUN_80057b80`.

## Matching order for the family

1. ~~`SetDepthQ`~~ — DONE (the spike).
2. `drawF3` — the family anchor: smallest renderer, establishes the macro set,
   the non-ABI entry spelling, and the loop shape.
3. The other 15 `draw*` — anchor-then-clone with per-primitive deltas.
4. The three twin pairs (920/984/1260 B — each pair is near-identical).
5. `FUN_80057b80` (3796 B, only 2 GTE commands — mostly ordinary C).
6. `DrawTMD` (no GTE opcodes; needs the pinned-register indirect-call setup).
