#include "common.h"
#include "main.exe.h"
#include "effect.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void SetBleeds(struct VECTOR *pos, short grange, short srange, short n, int time, long col);
 *     EFFECT.C:963, 11 src lines, frame 88 bytes, saved-reg mask 0xc0ff0000 (DEMO build -- see below)
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
 *     param stack+0   struct VECTOR * pos
 *     param $a1       short grange
 *     param $s5       short srange
 *     param $s6       short n
 *     param stack+16  int time
 *     param stack+20  long col
 *     reg   $fp       int time
 *     reg   $s7       long col
 *     stack sp+16     struct VECTOR npos
 *     stack sp+32     struct SVECTOR v
 *     reg   $a3       struct VECTOR * pos
 *     reg   $t0       int time
 *     reg   $s7       long col
 *     reg   $v1       struct BleedType * param
 *     reg   $a2       struct tag_EffectSlot * slot
 *     reg   $a0       int i
 *
 * Globals it touches, as the original declared them:
 *     extern struct tag_EffectSlot EffectSlot[200];
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — draft links 1004 bytes vs the 1108-byte target (26
 *   instructions SHORT). Default
 * build keeps the byte-identical INCLUDE_ASM stub.
 *
 * Progress made (each verified against the raw .s, all still true in the
 * current draft): the outer loop MUST be a real `do { if (n<=0) return; ...;
 * } while (1);` (not `while (n>0){...}`) — only the do-while(1) shape puts
 * the invariant setup (grange2/srange2's sign-extend, `pos`'s stack spill)
 * in the function's UNCONDITIONAL prologue, matching the target; a
 * `while(cond)` puts the same code in a conditional preheader reached only
 * once the loop is entered, one instruction later in the compiled output
 * (same root cause as SetSmoke's outer loop, see its header). `grange2 =
 * grange*2; srange2 = srange*2;` must be computed ONCE before the loop, not
 * recomputed inside it, for the same reason. The two `memset(&npos/&v, 0,
 * sizeof(...))` calls (dropped from an early draft; visible in the Ghidra
 * reference below) are load-bearing — without them the function is ~130
 * bytes shorter and `.pad`/unused fields aren't zeroed.
 *
 * Residual not yet resolved: target computes each of npos/v's THREE fields
 * into throwaway STACK SCRATCH slots (not registers) before a block-copy
 * into npos/v's own stack slot, then a SECOND block-copy into
 * `ef->param.bleed.pos/vec` — i.e. two copies per aggregate, not one.
 * Splitting the computation into scalar temps (`px,py,pz`/`svx,svy,svz`,
 * assigned into npos/v only at the end, matching Ghidra's own
 * local_38/34/30/2c naming showing SEPARATE scalars, not struct-field
 * writes) reproduced the register-vs-stack PRESSURE shape partially (one of
 * the three now spills, `long py` lands in a genuine callee-saved reg
 * instead) but not fully both aggregates — the residual is smaller
 * (944->1004 of 1108 bytes) but not closed. Root cause appears to be that
 * `time` (a stack-passed, single-use parameter) gets RELOADED on demand in
 * this draft (`lw t0,...(sp)` right at its one use) instead of occupying a
 * DEDICATED callee-saved register for the whole function like target's
 * `$fp` — one fewer register pinned down for the whole function leaves
 * `px`/`py`/`pz` a free register to live in instead of forcing all three to
 * spill to the scratch slots target uses. No source lever found to force
 * cc1 to keep a single-use stack parameter resident; this is the same
 * "allocator cost tie" class as SetSmoke's `$fp`-vs-`base` residual.
 * Parked per the cookbook's attempt cap; the field values, loop shape,
 * struct layout (BleedType, effect.h) and store order below are all
 * independently confirmed correct.
 *
 * Matching notes (verified against the raw .s):
 *  - `pos` is spilled straight to a stack slot at entry (`sw $a0,...($sp)`)
 *    and reloaded from there for each of pos->vx/vy/vz — NOT kept in a
 *    register across the whole function (unlike SetBleed/SetExplosion's
 *    pos/vect params).
 *  - `grange` is fully sign-extended (`sll 16/sra 16`) because it feeds a
 *    FULL-WORD store (npos is a VECTOR of longs); `srange` is used mostly
 *    RAW (just copied from its register, no sign-extend) because it only
 *    ever feeds a HALFWORD store (v is an SVECTOR) — the narrowing store
 *    discards any garbage upper bits, so cc1 doesn't bother widening it.
 *    `srange * 2` gets the one-step `sll 16 / sra 15` widen-and-scale fuse
 *    (combining the sign-extend with the *2 in a single shift amount).
 *  - Each of the six jitter fields (npos.vx/vy/vz from `grange`, v.vx/vy/vz
 *    from `srange`) is `base + delta` where `delta` defaults to `-range`
 *    and is overwritten by `rand() % (range*2) - range` when `range*2 > 0`
 *    — write the default BEFORE the guard, matching SetImpact's
 *    default-then-conditionally-overwritten idiom, not a ternary.
 *  - The `time` split (`half = time/2; rem = time-half; btime = half +
 *    (rem>0 ? rand()%rem : 0)`) feeds BleedType.time (u8) — matches the
 *    original Ghidra rendering literally.
 *  - The inner EffectSlot[200] pool scan is SetBleed.c's own hand-rolled
 *    `goto loop;` shape verbatim (count++ only on the NOT-found path,
 *    &dmy assigned after the loop, cursor-store inside the found branch) —
 *    reuse it exactly, just add `n = n - 1;` on both exits (found and
 *    give-up) before falling into the shared tail.
 *  - `EffectSlot`'s address is NOT hoisted out of the outer loop here
 *    (recomputed every iteration via lui/addiu) — unlike SetSmoke, there's
 *    no repeated-3x constant-divisor magic-multiply to justify caching a
 *    constant in a callee-saved register, and register pressure from
 *    grange/srange/n/time/col/pos leaves no obvious slack for it either.
 *  - Field store order into BleedType: pos (block copy), vec (block copy),
 *    r, g, time, b, mode, proc — the SAME order as SetBleed.c (r,g,time,b,
 *    mode), not the offset order (r,g,b,time,mode).
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetBleeds", SetBleeds);
#else

extern void DrawBleed(TEffectSlot *ef);
extern void *memset(void *s, int c, u32 n);

void SetBleeds(VECTOR *pos, short grange, short srange, short n, int time, long col)
{
    VECTOR npos;
    SVECTOR v;
    long px, py, pz;
    short svx, svy, svz;
    int grange2;
    int srange2;
    long d;
    int half;
    int rem;
    int btime;
    int idx;
    TEffectSlot *base;
    TEffectSlot *slot;
    int count;
    TEffectSlot *ef;
    BleedType *fp;

    grange2 = grange * 2;
    srange2 = srange * 2;
    do
    {
        if (n <= 0)
        {
            return;
        }
        memset(&npos, 0, sizeof(npos));
        d = -grange;
        if (grange2 > 0)
        {
            d = rand() % grange2 - grange;
        }
        px = pos->vx + d;

        d = -grange;
        if (grange2 > 0)
        {
            d = rand() % grange2 - grange;
        }
        py = pos->vy + d;

        d = -grange;
        if (grange2 > 0)
        {
            d = rand() % grange2 - grange;
        }
        pz = pos->vz + d;
        npos.vx = px;
        npos.vy = py;
        npos.vz = pz;

        memset(&v, 0, sizeof(v));
        d = -srange;
        if (srange2 > 0)
        {
            d = rand() % srange2 - srange;
        }
        svx = d;

        d = -srange;
        if (srange2 > 0)
        {
            d = rand() % srange2 - srange;
        }
        svy = d;

        d = -srange;
        if (srange2 > 0)
        {
            d = rand() % srange2 - srange;
        }
        svz = d;
        v.vx = svx;
        v.vy = svy;
        v.vz = svz;

        half = time / 2;
        rem = time - half;
        btime = half;
        if (rem > 0)
        {
            btime = rand() % rem + half;
        }

        base = EffectSlot;
        idx = CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_;
        slot = base + idx;
        count = 0;
    loop:
        idx = idx + 1;
        slot = slot + 1;
        if (199 < idx)
        {
            slot = base;
            idx = 0;
        }
        if (slot->proc == 0)
        {
            CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_ = idx + 1;
            if (199 < idx + 1)
            {
                CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_ = 0;
            }
            ef = slot;
            n = n - 1;
            goto found;
        }
        count = count + 1;
        if (199 < count)
        {
            ef = &dmy;
            n = n - 1;
            goto found;
        }
        goto loop;
    found:
        fp = &ef->param.bleed;
        fp->pos = npos;
        fp->vec = v;
        fp->r = col >> 16;
        fp->g = col >> 8;
        fp->time = btime;
        fp->b = col;
        fp->mode = 0;
        ef->proc = (void (*)())DrawBleed;
    } while (1);
}
#endif

// triage: MEDIUM — 277 insns, mul/div, 1 loop, 2 callees, ~0.08 to InsertConflict
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetBleeds(short grange,short srange,short n)
//
// {
//   AreaNodeType *pAVar1;
//   int iVar2;
//   long lVar3;
//   int iVar4;
//   int iVar5;
//   undefined2 in_register_00000012;
//   int *piVar6;
//   tag_EffectSlot *ptVar7;
//   int in_a3;
//   int iVar8;
//   int iVar9;
//   int iVar10;
//   int iVar11;
//   int iVar12;
//   int in_stack_00000010;
//   undefined4 in_stack_00000014;
//   AreaNodeType *local_38;
//   int local_34;
//   undefined4 local_30;
//   long local_2c;
//
//   piVar6 = (int *)CONCAT22(in_register_00000012,grange);
//   iVar12 = (int)srange;
//   iVar11 = iVar12 << 1;
//   iVar10 = (int)((uint)(ushort)n << 0x10) >> 0xf;
//   do {
//     if (in_a3 << 0x10 < 1) {
//       return;
//     }
//     memset((uchar *)&local_38,'\0',0x10);
//     iVar8 = *piVar6;
//     iVar4 = -iVar12;
//     if (0 < iVar11) {
//       iVar4 = rand();
//       if (iVar11 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar11 == -1) && (iVar4 == -0x80000000)) {
//         trap(0x1800);
//       }
//       iVar4 = iVar4 % iVar11 - iVar12;
//     }
//     local_38 = (AreaNodeType *)(iVar8 + iVar4);
//     iVar8 = piVar6[1];
//     iVar4 = -iVar12;
//     if (0 < iVar11) {
//       iVar4 = rand();
//       if (iVar11 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar11 == -1) && (iVar4 == -0x80000000)) {
//         trap(0x1800);
//       }
//       iVar4 = iVar4 % iVar11 - iVar12;
//     }
//     local_34 = iVar8 + iVar4;
//     iVar8 = piVar6[2];
//     iVar4 = -iVar12;
//     if (0 < iVar11) {
//       iVar4 = rand();
//       if (iVar11 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar11 == -1) && (iVar4 == -0x80000000)) {
//         trap(0x1800);
//       }
//       iVar4 = iVar4 % iVar11 - iVar12;
//     }
//     lVar3 = local_2c;
//     iVar2 = local_34;
//     pAVar1 = local_38;
//     local_30 = (AreaNodeType *)(iVar8 + iVar4);
//     iVar4 = (int)local_30;
//     memset((uchar *)&local_30,'\0',8);
//     if (iVar10 < 1) {
//       local_30 = (AreaNodeType *)CONCAT22(local_30._2_2_,-n);
//     }
//     else {
//       iVar8 = rand();
//       if (iVar10 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar10 == -1) && (iVar8 == -0x80000000)) {
//         trap(0x1800);
//       }
//       local_30 = (AreaNodeType *)CONCAT22(local_30._2_2_,(short)(iVar8 % iVar10) - n);
//     }
//     if (iVar10 < 1) {
//       local_30 = (AreaNodeType *)CONCAT22(-n,(undefined2)local_30);
//     }
//     else {
//       iVar8 = rand();
//       if (iVar10 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar10 == -1) && (iVar8 == -0x80000000)) {
//         trap(0x1800);
//       }
//       local_30 = (AreaNodeType *)CONCAT22((short)(iVar8 % iVar10) - n,(undefined2)local_30);
//     }
//     if (iVar10 < 1) {
//       local_2c = CONCAT22(local_2c._2_2_,-n);
//     }
//     else {
//       iVar8 = rand();
//       if (iVar10 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar10 == -1) && (iVar8 == -0x80000000)) {
//         trap(0x1800);
//       }
//       local_2c = CONCAT22(local_2c._2_2_,(short)(iVar8 % iVar10) - n);
//     }
//     local_38 = local_30;
//     local_34 = local_2c;
//     iVar8 = in_stack_00000010 / 2;
//     iVar9 = in_stack_00000010 - iVar8;
//     if (0 < iVar9) {
//       iVar5 = rand();
//       if (iVar9 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar9 == -1) && (iVar5 == -0x80000000)) {
//         trap(0x1800);
//       }
//       iVar8 = iVar5 % iVar9 + iVar8;
//     }
//     iVar5 = 0;
//     ptVar7 = EffectSlot + DAT_80097a3c;
//     iVar9 = DAT_80097a3c;
//     do {
//       iVar9 = iVar9 + 1;
//       ptVar7 = ptVar7 + 1;
//       if (199 < iVar9) {
//         iVar9 = 0;
//         ptVar7 = EffectSlot;
//       }
//       if (ptVar7->proc == (undefined **)0x0) {
//         DAT_80097a3c = iVar9 + 1;
//         if (199 < iVar9 + 1) {
//           DAT_80097a3c = 0;
//         }
//         goto LAB_80034944;
//       }
//       iVar5 = iVar5 + 1;
//     } while (iVar5 < 200);
//     ptVar7 = &dmy;
// LAB_80034944:
//     in_a3 = in_a3 + -1;
//     (ptVar7->param).blood.hint = pAVar1;
//     (ptVar7->param).blood.px = iVar2;
//     (ptVar7->param).blood.py = iVar4;
//     (ptVar7->param).blood.pz = lVar3;
//     (ptVar7->param).blood.scale = (long)local_38;
//     (ptVar7->param).blood.rotate = local_34;
//     (ptVar7->param).bleed.r = (uchar)((uint)in_stack_00000014 >> 0x10);
//     (ptVar7->param).bleed.g = (uchar)((uint)in_stack_00000014 >> 8);
//     (ptVar7->param).bleed.time = (uchar)iVar8;
//     (ptVar7->param).bleed.b = (uchar)in_stack_00000014;
//     (ptVar7->param).bleed.mode = '\0';
//     ptVar7->proc = (undefined **)DrawBleed;
//   } while( true );
// }
