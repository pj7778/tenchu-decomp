#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short Think1random(void);
 *     THINK_1.C:74, frame 32 bytes, saved-reg mask 0x80030000 (DEMO build -- see below)
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate.
 * A ZERO-locals record is unverified, not a claim that the function has none:
 * vfree lists zero locals yet its byte-matched source needs seven.
 * The frame size and saved-reg mask above are the DEMO's: retail often needs
 * FEWER callee-saved registers (measured: Think1random exact; Think1chase's
 * 0x800f0000 = s0-s3+ra vs retail's s0,s1,ra). Treat them as an upper bound
 * and a hint at how many values stay live, never as a spec. The asm wins.
 * Locals:
 *     reg   $a1       long xx
 *     reg   $v1       long zz
 *     reg   $s1       short pad
 *     reg   $a1       long vx
 *     reg   $v1       long vz
 *     reg   $v0       short deg
 *     reg   $s0       short pad
 *
 * Globals it touches, as the original declared them:
 *     extern short Attrib;
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 10 of 372 bytes differ (right length, 93
 * instructions both sides; every differing byte is a register-only tie, no
 * wrong/extra/missing instruction).
 *
 * Think1random (0x8002c0c4, 0x174 bytes) — think-handler, same "think" TU as
 * Think1sleep.c/Think1chase.c/Think1trace.c. On the first tick of a new
 * "random walk" cycle (actcnt just rolled over to 1), picks a fresh chase
 * target near the character's spawn point (some_x/z_position +/- up to
 * 5000, via rand()%10000-5000 on each axis). On later ticks, once within
 * 1000 units of the chase target on BOTH axes, resets actcnt to 0 (so the
 * next tick re-rolls); otherwise steers towards it via
 * turn_towards_player_, unless Attrib bit 0x400 is set (in which case it
 * also just resets — the same "reached/blocked" reset as the close case).
 *
 * `FUN_8002b990` (Ghidra's/m2c's placeholder name for this call site) is
 * `turn_towards_player_` at the SAME address (0x8002b990, see
 * config/symbols.main.exe.txt) — not a separate unmatched function.
 * Ghidra's own rendering shows the call with NO arguments
 * (`sVar1 = FUN_8002b990();`), but the asm passes the just-computed
 * (chase - locate) diffs in $a0/$a1 (the abs-value clamp that decides
 * whether to reach the call negates a COPY in $v0, leaving the raw diffs
 * in $a0/$a1 untouched) — the m2c/Ghidra call-arg-undercount tell from the
 * cookbook, confirmed against the raw `.s` (matches Think4contact/
 * Think4chase's explicit `FUN_8002b990(iVar5,iVar4)` sibling call).
 *
 * `actcnt` is re-narrowed to u8 with a defensive `andi 0xff` right after the
 * `+1`/store-back, even though the store already truncates — the
 * u8-local-forces-a-mask rule (the compare needs the masked value, the
 * store doesn't).
 *
 * `dx`/`dz` (the chase-locate diffs) and `adx`/`adz` (their abs values) are
 * separate locals: the asm computes abs into a fresh register ($v0) while
 * leaving the raw diffs live in $a0/$a1 for the later turn_towards_player_
 * call — same "abs_degree stays separate from degree" idiom as
 * Think1trace.c.
 *
 * RESOLVED THIS SESSION (14 -> 10 bytes, two independent fixes):
 *  - `adz`'s abs (`(dz >= 0) ? dz : -dz`) already matched cleanly. Applying
 *    the SAME cookbook rule ("abs reaches abssi2 only as the GE ternary")
 *    to `adx` alone (`adx = (dx >= 0) ? dx : -dx;` in place of the
 *    `adx = dx; if (adx < 0) adx = -adx;` if/else form) got the RIGHT
 *    `move/negu-self` shape but made the function 4 bytes SHORT — it let
 *    sched1/reorg additionally hoist `dz`'s subtraction into the outer
 *    `if (adx < 1000)` guard's delay slot (target leaves that slot a plain
 *    `nop`; confirmed via `rtldump --pass all`: at `.sched2` both this
 *    draft's and the if/else draft's `dz` subtraction (insn 140) already
 *    sits BEFORE the abs pattern in program order, so the 4-byte loss
 *    happens later, during the abssi2 pattern's own split/delay-slot fill —
 *    a genuine cross-cutting scheduling side effect of the ternary, not a
 *    statement-order artifact: reordering `dz`'s subtraction relative to
 *    `adx`'s ternary in the SOURCE changed nothing).
 *  - Fix: reuse `adx` as a throwaway scratch for `vx` BEFORE its abs role
 *    (`adx = Me_THINK_C->some_kind_of_current_position->vx; dx = dx - adx;`
 *    ... `adx = (dx >= 0) ? dx : -dx;`) instead of reading
 *    `Me_THINK_C->some_kind_of_current_position->vx` inline in the
 *    subtraction. This EXTENDS `adx`'s pseudo live range (a `global.c`
 *    priority lever, cookbook "Register allocation steering" — same
 *    mechanism as the documented ballast rule, applied to an UNRELATED
 *    scratch role instead of a hoisted constant), which absorbs the extra
 *    pressure the ternary introduces: the 4-byte shortfall disappears (back
 *    to 93 instructions / 372 bytes) AND, as a bonus, the previously-tied
 *    `rand() % 10000` `mfhi $a3` vs `$a2` register (both occurrences) now
 *    matches too — confirming the original header's own diagnosis that this
 *    was "a whole-function global-allocation artifact", fixable by shifting
 *    ANY allocno's priority, not specifically that snippet. Found by testing
 *    a `tools/permute.py` low-score candidate (`adz = <vx>; dx = dx - adz;`,
 *    score 55->50) directly, then hand-varying WHICH local absorbs the
 *    scratch read (`adz` cross-contaminates `adz`'s own later abs, landing
 *    it in `$a2` instead of `$v0` — net worse; `result` is semantically
 *    unsafe, the reset path never reassigns it so borrowing it changes
 *    real behaviour; `adx` is the only safe, clean choice: its "real" role
 *    (a fresh assignment) begins before it's ever read again).
 *
 * RESIDUAL (10 bytes / 7 instructions, all register-only, no wrong
 * instruction anywhere — `asmdiff.py` confirms 93 vs 93 instructions):
 *  - The `else` branch's three loads (`some_kind_of_current_position`,
 *    `some_other_x_position`, `some_other_z_position`) land in
 *    {v1,a2,a1}/{a0,v0,v1} depending on whether `locate` is a named local
 *    or inlined — tried both (again, on top of the fix above), byte-
 *    identical to each other, both a register-only tie vs. target
 *    (confirmed: introducing a named `VECTOR *locate` changes nothing).
 *  - `adx`'s abs now has the RIGHT shape (`move/negu-self`, matching
 *    `adz`'s and Think1trace.c's idiom) but lands in `$a2` instead of the
 *    target's `$v0` throughout (`move`, `negu`, `slti` all shifted) — a
 *    pure `global.c` colouring tie, downstream of the same live-range
 *    extension that fixed the other two issues; tried `adz`/`result`/a
 *    fresh dedicated local as the `vx` scratch instead of `adx` (see
 *    above) — all worse or unsafe.
 * Tried and rejected (all no better than the 10-byte state above): a named
 * `locate` local; a `do{}while(0)` wrapper around the `if(actcnt==1)` body
 * (priority-boost lever). `tools/permute.py Think1random -- --stop-on-zero
 * -j4`, THREE bounded runs total across sessions (~400s, ~180s hitting an
 * internal permuter crash on `rand()` — `KeyError: 'rand'`, not a bug in
 * this source — and a 240s/~28000-iteration run on this session's improved
 * 10-byte baseline): base score dropped 70->55 confirming the improvement,
 * but the only sub-baseline candidate (score 40, 1 occurrence in 28000)
 * rewrote the ternary as `new_var2 = dx >= 0; adx = (new_var = new_var2 ?
 * dx : -dx);` — a nonsensical assignment-expression restructuring, not
 * something the original 1990s source plausibly contains; not adopted.
 * Parked per the cookbook's attempt-cap/sub-C-level-residual guidance.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/Think1random", Think1random);
#else
extern s16 Attrib;

s16 Think1random(void)
{
    u8 actcnt;
    s32 result;

    actcnt = Me_THINK_C->actcnt + 1;
    Me_THINK_C->actcnt = actcnt;
    result = 0;
    if (actcnt == 1)
    {
        Me_THINK_C->some_other_x_position = Me_THINK_C->some_x_position + rand() % 10000 - 5000;
        Me_THINK_C->some_other_z_position = Me_THINK_C->some_z_position + rand() % 10000 - 5000;
    }
    else
    {
        s32 dx, dz;
        s32 adx, adz;

        dx = Me_THINK_C->some_other_x_position;
        adx = Me_THINK_C->some_kind_of_current_position->vx;
        dz = Me_THINK_C->some_other_z_position;
        dx = dx - adx;
        dz = dz - Me_THINK_C->some_kind_of_current_position->vz;
        adx = (dx >= 0) ? dx : -dx;
        if (adx < 1000)
        {
            adz = (dz >= 0) ? dz : -dz;
            if (adz < 1000)
            {
                goto reset;
            }
        }
        if (Attrib & 0x400)
        {
        reset:
            Me_THINK_C->actcnt = 0;
        }
        else
        {
            result = turn_towards_player_(dx, dz);
        }
    }
    return result;
}
#endif
