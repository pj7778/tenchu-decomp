#include "common.h"
#include "main.exe.h"
#include "effect.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void SetSmoke(struct VECTOR *pos, struct SVECTOR *vect, short n, short time);
 *     EFFECT.C:835, 21 src lines, frame 56 bytes, saved-reg mask 0x80ff0000 (DEMO build -- see below)
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
 *     param $s5       struct VECTOR * pos
 *     param $s6       struct SVECTOR * vect
 *     param $s7       short n
 *     param $s3       short time
 *     reg   $s4       short i
 *     reg   $s1       struct tag_EffectSlot * slot
 *     reg   $s0       struct SmokeType * smoke
 *     reg   $a0       int i
 *
 * Globals it touches, as the original declared them:
 *     extern struct tag_EffectSlot EffectSlot[200];
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — draft links 780 bytes vs the 788-byte target (2
 *   instructions SHORT), a register-pressure tie. The draft
 * below reproduces every field computation and the pool-scan loop exactly
 * (only ~11 diff blocks total, mostly this one root cause) but ties on a
 * register-allocation choice: the target dedicates a SEPARATE callee-saved
 * register ($fp) to the RAW `n` parameter and re-derives its sign-extended
 * form fresh every outer-loop iteration via a cheap `sll $v0,i,16 / sll
 * $v1,n,16 / slt` double-left-shift compare (no `sra`, since only ordering
 * matters) — freeing ANOTHER callee-saved register ($s7) to cache
 * `&EffectSlot` across the whole function. Our draft's cc1 instead folds
 * `n`'s widening into ONE persistent register (a real `sll+sra`, done once)
 * which is cheaper by itself but leaves no register free for `base`, so
 * `&EffectSlot` gets recomputed (lui+addiu) every iteration instead —
 * statically equal cost (2 instructions either way: save+restore $fp vs.
 * recompute lui+addiu), so this is a genuine allocator tie, not a source
 * bug. Tried and reverted: hoisting `base = EffectSlot;` above the loop
 * (regressed to +4 bytes), De Morgan / operand-swap rewrites of the `n<=i`
 * guard (`if (i >= n)`, `if (!(i < n))` — both regressed further, to +8/+52
 * bytes and scrambled unrelated register assignments), declaration-order
 * changes (no effect). A bounded permuter run (~1350 iterations, -j4, full
 * 420s budget) found no zero. Per the cookbook's sub-C-level early-stop,
 * parked here — every other field/loop/expression choice in this file is
 * confirmed correct against the raw .s.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetSmoke", SetSmoke);
#else

/*
 * Matching notes (all verified against the raw .s):
 *  - Same EffectSlot[200] pool search shape as SetExplosion (do{...}while
 *    (count<200), ef=&dmy AFTER the loop, count++ BEFORE the proc test),
 *    but wrapped in an OUTER `do { count = 0; if (n <= i) return; ...scan...;
 *    i++; ...fields...; } while (1);` that spawns n particles — `count = 0;`
 *    is literally the first statement, landing in the entry test's branch
 *    delay slot (confirmed: `beqz $v0,exit / addu $a1,zero,zero` in the
 *    delay slot).
 *  - `i` must be `short` (matches PSX.SYM's `reg $s4 short i`): the outer
 *    guard compiles as `sll $v0,i,16 / sll $v1,n,16 / slt` (both operands
 *    shifted into the upper half), the double-shift idiom cc1 uses to
 *    compare two HImode pseudos — declaring `i` `int` would drop the shifts.
 *  - `SmokeType` reuses ExplosionType's vec@0x0/pos@0x8/rotate@0x18/scale@0x1c
 *    layout (proven by tools/access.py --order) plus mode@0x20/bright@0x21/
 *    unk22@0x22 (cross-checked against SetSmokeS.c's identical field usage,
 *    itself unmatched but sharing the same union view).
 *  - `scale = rand() % 0x2000 + 0x1000;` stored FIRST, before `rotate`,
 *    which is the standard SetExplosion/SetImpact field-fill idiom.
 *  - Each `vec.v{x,y,z}` jitter is `vect->v{x,y,z} + (rand() % 100 - 50)`
 *    — NOT `vect->vx - 50 + rand() % 100` — the fold-reassociation rule:
 *    `A + (B - C)` is what reassociates INTO the target's actual compiled
 *    order (materialize `vect->vx - 50` first, add the mod-100 result
 *    after the call returns).
 *  - `mode = time + rand() % 0xa0;` stores as a byte (mode field is u8);
 *    `bright` then reads `smoke->mode` back via a fresh `lbu` (the compiler
 *    reloads across the intervening `rand()` call) — do NOT cache mode's
 *    computed value in a local, reference `smoke->mode` again so the
 *    reload happens naturally.
 *  - `bright = (smoke->mode - 1) - (time / 2 + rand() % time);` — the raw
 *    asm is `addiu $v0,$v0,-1`, i.e. a literal `- 1`, not Ghidra's
 *    `+ 0xff` (same bit pattern once truncated to a byte at the final
 *    store, but only `- 1` reproduces the actual instruction).
 *  - `rand() % time` is the ONLY variable-divisor division in the function
 *    (needs `--expand-div`: see Build.hs/permute.py `SetSmoke` entries) —
 *    every other `%`/`rand()` use is a compile-time-constant divisor.
 */
extern void DrawSmoke(TEffectSlot *ef);

void SetSmoke(VECTOR *pos, SVECTOR *vect, short n, short time)
{
    short i;
    int idx;
    TEffectSlot *base;
    TEffectSlot *slot;
    int count;
    TEffectSlot *ef;
    SmokeType *smoke;
    int r;

    i = 0;
    do
    {
        count = 0;
        if (n <= i)
        {
            return;
        }
        base = EffectSlot;
        idx = CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_;
        slot = base + idx;
        do
        {
            idx = idx + 1;
            slot = slot + 1;
            if (199 < idx)
            {
                slot = base;
                idx = 0;
            }
            count = count + 1;
            if (slot->proc == 0)
            {
                CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_ = idx + 1;
                if (199 < idx + 1)
                {
                    CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_ = 0;
                }
                ef = slot;
                goto found;
            }
        } while (count < 200);
        ef = &dmy;
found:
        smoke = &ef->param.smoke;
        r = rand();
        smoke->scale = r % 0x2000 + 0x1000;
        smoke->rotate = (rand() % 360) * 0x1000;
        smoke->pos.vx = pos->vx;
        smoke->pos.vy = pos->vy;
        smoke->pos.vz = pos->vz;
        smoke->vec.vx = vect->vx + (rand() % 100 - 50);
        smoke->vec.vy = vect->vy + (rand() % 100 - 50);
        smoke->vec.vz = vect->vz + (rand() % 100 - 50);
        smoke->mode = time + rand() % 0xa0;
        r = rand();
        i = i + 1;
        smoke->unk22 = 0;
        smoke->bright = (smoke->mode - 1) - (time / 2 + r % time);
        ef->proc = (void (*)())DrawSmoke;
    } while (1);
}
#endif

// triage: MEDIUM — 197 insns, mul/div, 1 loop, 1 callees, ~0.09 to ReqItemNingyo
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetSmoke(VECTOR *pos,SVECTOR *vect,short n,short time)
//
// {
//   int iVar1;
//   int iVar2;
//   tag_EffectSlot *ptVar3;
//   int iVar4;
//
//   iVar4 = 0;
//   do {
//     iVar2 = 0;
//     if ((int)((uint)(ushort)n << 0x10) <= iVar4 << 0x10) {
//       return;
//     }
//     ptVar3 = EffectSlot + DAT_80097a3c;
//     iVar1 = DAT_80097a3c;
//     do {
//       iVar1 = iVar1 + 1;
//       ptVar3 = ptVar3 + 1;
//       if (199 < iVar1) {
//         iVar1 = 0;
//         ptVar3 = EffectSlot;
//       }
//       iVar2 = iVar2 + 1;
//       if (ptVar3->proc == (undefined **)0x0) {
//         DAT_80097a3c = iVar1 + 1;
//         if (199 < DAT_80097a3c) {
//           DAT_80097a3c = 0;
//         }
//         goto LAB_8003379c;
//       }
//     } while (iVar2 < 200);
//     ptVar3 = &dmy;
// LAB_8003379c:
//     iVar1 = rand();
//     iVar2 = iVar1;
//     if (iVar1 < 0) {
//       iVar2 = iVar1 + 0x1fff;
//     }
//     (ptVar3->param).smoke.scale = iVar1 + (iVar2 >> 0xd) * -0x2000 + 0x1000;
//     iVar2 = rand();
//     (ptVar3->param).smoke.rotate = (iVar2 % 0x168) * 0x1000;
//     (ptVar3->param).blood.py = pos->vx;
//     (ptVar3->param).blood.pz = pos->vy;
//     (ptVar3->param).blood.scale = pos->vz;
//     iVar2 = rand();
//     (ptVar3->param).smoke.vec.vx = vect->vx + -0x32 + (short)iVar2 + (short)(iVar2 / 100) * -100;
//     iVar2 = rand();
//     (ptVar3->param).smoke.vec.vy = vect->vy + -0x32 + (short)iVar2 + (short)(iVar2 / 100) * -100;
//     iVar2 = rand();
//     (ptVar3->param).smoke.vec.vz = vect->vz + -0x32 + (short)iVar2 + (short)(iVar2 / 100) * -100;
//     iVar2 = rand();
//     (ptVar3->param).blood.mode = (char)time + (char)iVar2 + (char)(iVar2 / 0xa0) * '`';
//     iVar2 = rand();
//     iVar1 = (int)((uint)(ushort)time << 0x10) >> 0x10;
//     if (iVar1 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar1 == -1) && (iVar2 == -0x80000000)) {
//       trap(0x1800);
//     }
//     iVar4 = iVar4 + 1;
//     *(undefined1 *)((int)&ptVar3->param + 0x22) = 0;
//     (ptVar3->param).blood.bright =
//          ((ptVar3->param).blood.mode + 0xff) -
//          ((char)(iVar1 - ((int)((uint)(ushort)time << 0x10) >> 0x1f) >> 1) + (char)(iVar2 % iVar1));
//     ptVar3->proc = (undefined **)DrawSmoke;
//   } while( true );
// }
