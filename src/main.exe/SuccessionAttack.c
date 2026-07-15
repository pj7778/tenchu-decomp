#include "common.h"
#include <psxsdk/libgs.h>
#include "game_types.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * static short SuccessionAttack(long dist, short deg);
 *     THINK_3.C:247, 15 src lines, frame 24 bytes, saved-reg mask 0x80010000 (DEMO build -- see below)
 *
 * Original parameters and locals (the demo COUNT and TYPES are high-value
 * codegen evidence, not a retail spec: an earlier-build helper/API change
 * can replace either). Retail access widths and callee ABI win. A repeated
 * name is a nested-block scope, not a duplicate.
 * A ZERO-locals record is unverified, not a claim that the function has none:
 * vfree lists zero locals yet its byte-matched source needs seven.
 * The frame size and saved-reg mask above are the DEMO's: retail often needs
 * FEWER callee-saved registers (measured: Think1random exact; Think1chase's
 * 0x800f0000 = s0-s3+ra vs retail's s0,s1,ra). Treat them as an upper bound
 * and a hint at how many values stay live, never as a spec. The asm wins.
 * Locals:
 *     param $a0       long dist
 *     param $a1       short deg
 *
 * Globals it touches, as the original declared them:
 *     extern struct BattleType BattleDB[78];
 *     extern long Distance;
 *     extern short Degree;
 *     extern short EngageLevel;
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 6 of 268 bytes differ (right length: matchdiff's
 * whole-image count equals the function-local count). Was 25 bytes, then 8;
 * the current source fixes the compare-result register too. The remaining
 * pair is a dump-confirmed delay-slot reorganization choice (see RESIDUAL).
 *
 * SuccessionAttack (0x8002fabc, 0x10c bytes) — same "think" TU as
 * Think3chase.c/Think3escape.c/Think3firstattack.c/Think1trace.c (s16
 * return; gp-relative Me_THINK_C/Distance/Degree/EngageLevel — see gpsyms).
 * Called (as a static helper, per PSX.SYM) from Think3area/Think3attack/
 * Think3hitaway to decide whether the current attack motion may chain into
 * a follow-up.
 *
 * Guard: only continues if the character's current animation frame
 * (Me_THINK_C->something_about_current_animation->frames_since_animation_
 * start, the same field Think1sleep.c proves) equals BattleDB[idx].contfrm
 * — `idx` is Me_THINK_C's field @0x8C (already proven
 * `index_s32o_animation_collection` by another matched function; Ghidra's
 * own name for it here is "warid" — plausible, but not adopted without
 * `tools/callmatch.py --verify`).
 *
 * `BattleType` is declared locally (not a shared header) straight from
 * `reference/psxsym-types.h`'s proven 8-short layout — `contfrm` @0x8, the
 * offset the asm's `lh v0,8(v0)` confirms after the `sll v0,v0,4` (0x10
 * stride) index scale.
 *
 * The `iVar1 % iVar2` (`rand() % (EngageLevel+1)`) division-guard trap
 * sequence (`break 7`/`break 6`) needs maspsx `--expand-div` for this file
 * (added to Build.hs's `extra`/permute.py's `MASPSX_EXTRA` — same lever as
 * Think3escape/GetAreaMapLevel/bow_shoot_logic): WITHOUT it the trap guard
 * is silently dropped from the assembled output even though cc1 emitted it,
 * a ~150-byte-diff red herring that looks like a missing C construct.
 *
 * `uVar3` must stay 16-bit (NOT the `u8` autorules suggests, which "wins" by
 * 2 bytes but is a FALSE WIN: `uVar3 = 0x8000;`/`= 0x2000;` truncate to 0 in
 * a u8, an outright wrong value — reject per the cookbook's "never accept
 * an autorules win that changes what a value actually holds" caveat, this
 * time for a plain local rather than a struct field).
 *
 * `int d = deg;` declared as the FIRST statement inside the `Distance<dist`
 * block (not `deg` used inline) fixed the function's LENGTH (was 1
 * instruction/4 bytes short): the fallthrough block's first statement gets
 * hoisted into the OUTER guard branch's delay slot, and only `deg`'s own
 * sign-extension — not `Degree`'s load — is independent enough of the
 * ABS-compute to serve as that filler when it is the textually-first
 * reference.
 *
 * FOUR REAL STRUCTURAL FIXES applied across the two focused sessions, found by
 * decoding the target's raw instructions address-by-address (not guessing):
 *
 * 1. The tail's `if/else` at LAB_8002fb84 has its arms SWAPPED relative to
 *    Ghidra's rendering: the target's `bnez`/fallthrough shape decodes as
 *    `if (Degree >= 0x12d) { uVar3 = 0x2000; } else { uVar3 |= 0x80; if
 *    (Degree < -300) { uVar3 = -0x8000; } else { goto ret; } } uVar3 |= 0x80;
 *    ret: return uVar3;` — i.e. Ghidra's literal `if (Degree<0x12d){ if
 *    (-0x12d<Degree) return 0x80; ... }` is the INVERSE condition with the
 *    bodies swapped, PLUS the `return 0x80;` is really a `goto` that skips
 *    a SECOND, later `uVar3 |= 0x80;` (the delay slot of the skip-edge's
 *    `beqz` executes an unconditional `ori s0,s0,0x80` every time that arm
 *    is even considered, per MIPS delay-slot semantics — decode the
 *    disassembly with delay slots in mind, not the pseudo-linear reading).
 *    Getting the if/else polarity right fixed the branch layout AND, via a
 *    named `ret:` label + `goto` (not a second literal `return uVar3;` —
 *    two textually-identical `return` tails do NOT cross-jump-merge in this
 *    cc1; route the second through a `goto` to a single shared one, per the
 *    cookbook's Shared-tails section) the shared-return register colouring.
 * 2. `uVar3` must be `s16` (not `u16`): assigning the literal `-0x8000`
 *    (0x8000's bit pattern) to a `u16` still folds to the UNSIGNED
 *    representation internally, so cc1 emits `li s0,0x8000` (`ori`) instead
 *    of the target's `li s0,-32768` (`addiu`) — same 4 bytes, wrong opcode.
 *    `s16` reproduces the target's `addiu` encoding exactly.
 * 3. Store `iVar1 < d` back into `iVar1` before testing it. This deliberately
 *    reuses Degree's dying carrier, reproducing the target's `slt v1,...` and
 *    `bnez v1` instead of allocating a fresh `$v0` (8 -> 6 bytes).
 * 4. Load Degree into the single-set `raw` pseudo, then assign the ABS result
 *    to `iVar1` in both branch arms. This is not cosmetic: GCC 2.8.1's
 *    pre-reload `adjust_priority` dynamically promotes a single-set birthing
 *    pseudo to LAUNCH priority. It makes `.sched` and `.sched2` emit the
 *    target pre-reorg order `sll; lh Degree; sra; bgez`, while preserving the
 *    target hard-register colouring.
 *
 * RESIDUAL (6 bytes, same length): only 0x8002fb1c and 0x8002fb24 differ.
 * Target has `sra v0,v0,16; bgez v1,L; nop`; ours has `nop; bgez v1,L; sra`.
 * The `.sched` and `.sched2` dumps are now target-ordered, but `.dbr` shows
 * `fill_simple_delay_slots` removing the immediately preceding `sra` and
 * placing it in `bgez`'s slot. The official GCC 2.8.1 `reorg.c` explains the
 * result: its backward scan accepts this one-instruction arithmetic pattern
 * and stops only at a label/jump/barrier/asm or a real resource conflict.
 * Because the target's visible `sra v0` and `bgez v1` are independent, an
 * invisible retained RTL boundary or dependency in the original expansion is
 * the remaining likely difference.
 *
 * Focused rejects: moving declarations/assignments, an extra raw copy,
 * manual two-step sign extension, in-place `d` conversion, branch polarity,
 * ternary/goto spellings, and empty one-shot loops were flat or worsened
 * allocation/length. A preserved-label probe was flattened before reorg;
 * `register volatile int d` added 12 bytes, a real ABS loop added 8, and a
 * one-iteration loop flattened back to CURRENT(6). The earlier bounded
 * permuter found no zero. Do not rerun broad source permutations: the next
 * credible lead needs direct evidence for the original RTL boundary or an
 * exact plain-C dependency that survives through final jump optimization.
 */
extern character_state *Me_THINK_C;
extern s32 Distance;
extern s16 Degree;
extern s16 EngageLevel;
extern int rand(void);

typedef struct
{
    s16 mid;
    s16 power;
    s16 atks;
    s16 atke;
    s16 contfrm;
    s16 revise;
    s16 ilus;
    s16 ilue;
} BattleType;

extern BattleType BattleDB[];

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SuccessionAttack", SuccessionAttack);
#else /* NON_MATCHING */

short SuccessionAttack(long dist, short deg)
{
    int iVar1;
    int iVar2;
    s16 uVar3;

    uVar3 = 0;
    if (Me_THINK_C->something_about_current_animation->frames_since_animation_start !=
        BattleDB[Me_THINK_C->index_s32o_animation_collection].contfrm)
    {
        return 0;
    }
    if (Distance < dist)
    {
        int d;
        int raw;

        d = deg;
        raw = (int)Degree;
        if (raw < 0)
        {
            iVar1 = raw;
            iVar1 = -iVar1;
        }
        else
        {
            iVar1 = raw;
        }
        iVar1 = iVar1 < d;
        if (iVar1) goto LAB_8002fb84;
    }
    iVar1 = rand();
    iVar2 = EngageLevel + 1;
    if (iVar1 % iVar2 != 0)
    {
        return uVar3;
    }
LAB_8002fb84:
    if (Degree >= 0x12d)
    {
        uVar3 = 0x2000;
    }
    else
    {
        uVar3 |= 0x80;
        if (Degree < -300)
        {
            uVar3 = -0x8000;
        }
        else
        {
            goto ret;
        }
    }
    uVar3 |= 0x80;
ret:
    return uVar3;
}

#endif /* NON_MATCHING */
