#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern long GameClock;
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 55 of 416 bytes differ; RIGHT LENGTH (104
 * instructions both sides, confirmed by asmdiff: no inserts/deletes, only
 * operand/register replacements in 2 symmetric clusters, one per jittered
 * axis). Each cluster is the SAME residual: the degenerate arm's
 * `x - dx`/`z - dz` subtraction — computed from values already loaded
 * before the `blez` branch — gets speculatively hoisted by cc1's reorg
 * into the branch's OWN delay slot in this build (executed unconditionally,
 * harmless on the non-degenerate path since it's overwritten there), while
 * the target leaves that delay slot a bare `nop` and computes the
 * subtraction only at the degenerate label itself. Tried and confirmed
 * NOT the lever: swapping which operand is written first in the
 * subtraction (`-dx + x` vs `x - dx`), and routing the rand-path result
 * through a named temp before the store (both left the byte diff
 * unchanged) — a genuine reorg/scheduler choice below the C level, not a
 * source-shape question. `tools/autorules.py` found no improving edit.
 * `tools/permute.py` ran two bounded passes (~260s then ~200s,
 * `--stop-on-zero -j4`); its best candidate (score 395, own weighted
 * metric) only wrapped the Z-axis rand result through a redundant temp
 * plus unrelated dead stores — re-verified with `matchdiff.py`: applying
 * just the temp-wrap left the byte diff at 55/416, unchanged, confirming
 * the cookbook's "a permuter winner can carry red herrings" warning.
 * Parked per the sub-C-level early-stop.
 *
 * FUN_8004d6d4 (0x8004d6d4, 0x1A0 bytes) — periodic smoke/splash-puff think
 * function (message-style `(this, msg)`, no direct `jal` callers found —
 * reached through a function-pointer table). Same 3-way dispatch shape as
 * the neighbouring FUN_8004c59c (same TU): msg==0 resets a "played" byte
 * flag at `this+0x15`; msg<4 is otherwise ignored; msg>=4 fires once
 * `played==0`, then unconditionally spawns effects at a jittered position
 * (X and Z each jittered independently by a per-object range at
 * `this+0x1C`/`this+0x20`; Y is untouched) and, on even `GameClock` frames,
 * calls SetSmoke (a small upward puff) and SetSplash (a ground splash) at
 * that position — note this branch has NO reschedule/"next" field the way
 * FUN_8004c59c does; it fires on every qualifying call.
 *
 * Matching notes:
 *  - Dispatch shape identical to FUN_8004c59c: `if (arg1==0) goto reset;
 *    if (3<arg1) goto normal; return; reset: ...; normal: ...` — the
 *    explicit `goto`s (not `if/else`) are load-bearing: cc1 canonicalises
 *    a `return`-ending guard body as the FALLTHROUGH regardless of
 *    if/else/goto spelling for a 2-way split, so the "short returning
 *    case falls through, both real bodies are forward-jump targets"
 *    shape needs the "two independent if (cond) goto L;" cookbook recipe
 *    (test the ORIGINAL condition to `reset`, test the ORIGINAL
 *    complementary-range condition to `normal`, bare `return;` as the
 *    fallthrough short case) — an `if (arg1<4) return;` inline-return
 *    spelling for the second test does NOT reproduce it (verified on
 *    FUN_8004c59c: only `if (3 < arg1) goto normal;` placed reset between
 *    the two guard tests and normal's continuation, matching the target).
 *  - Each axis's jitter is ONE expression per branch, not a shared
 *    trailing `+x`: `vx = x - dx;` (degenerate, dx<=0) vs
 *    `vx = x + (rand() % (dx << 1) - dx);` (normal) — Ghidra renders a
 *    shared `local_28.vx = iVar3 + local_28.vx;` after the if/else, but
 *    the target's single `subu`/`addu` fusing x with the branch's own
 *    result means each arm computes the FULL field directly.
 *  - The degenerate test is `(dx << 1) < 1`, not `dx <= 0` — the target
 *    tests the DOUBLED value (feeding the same shift into the `blez`),
 *    not the raw field. Written here as the DE MORGAN'd `1 <= (dx << 1)`
 *    with the rand-path as the "then" (so it's the fallthrough/inline
 *    body, matching the target's placement) and the degenerate case as
 *    the "else" (the `blez`-reached forward-jump target) — the plain
 *    `if ((dx<<1)<1) {degenerate} else {rand}` spelling swaps which body
 *    is inline vs jump-target (same instructions, wrong layout, wrong
 *    length by the register-pressure cascade this cookbook already notes
 *    for FUN_8004c59c's dispatch).
 *  - Needs maspsx's --expand-div (rand() % (dx<<1) divides by a runtime
 *    value, twice, once per axis); no explicit trap() calls belong in the
 *    C.
 */

extern s32 GameClock;
extern s32 rand(void);
extern void SetSmoke(VECTOR *pos, SVECTOR *vect, s16 n, s16 time);
extern void SetSplash(VECTOR *pos, s16 sx, s16 sy, s32 speed);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8004d6d4", FUN_8004d6d4);
#else

void FUN_8004d6d4(u8 *arg0, u32 arg1)
{
    VECTOR pos;
    SVECTOR dir;
    s32 x, z;

    if (arg1 == 0) goto reset;
    if (3 < arg1) goto normal;
    return;

reset:
    *(arg0 + 0x15) = 0;
    return;

normal:
    if (*(arg0 + 0x15) != 0) return;

    x = *(s32 *)(arg0 + 4);
    if (1 <= (*(s32 *)(arg0 + 0x1C) << 1)) {
        pos.vx = x + (rand() % (*(s32 *)(arg0 + 0x1C) << 1) - *(s32 *)(arg0 + 0x1C));
    } else {
        pos.vx = -*(s32 *)(arg0 + 0x1C) + x;
    }

    pos.vy = *(s32 *)(arg0 + 8);
    z = *(s32 *)(arg0 + 0xC);
    if (1 <= (*(s32 *)(arg0 + 0x20) << 1)) {
        pos.vz = z + (rand() % (*(s32 *)(arg0 + 0x20) << 1) - *(s32 *)(arg0 + 0x20));
    } else {
        pos.vz = z - *(s32 *)(arg0 + 0x20);
    }

    if ((GameClock & 1) == 0) {
        dir.vx = 0;
        dir.vy = -400;
        dir.vz = 0;
        pos.vy = pos.vy + 2000;
        SetSmoke(&pos, &dir, 1, 1);
        pos.vy = pos.vy - 2000;
        SetSplash(&pos, 0x4000, 0x2000, 10);
    }
}
#endif
