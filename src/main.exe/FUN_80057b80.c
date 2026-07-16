#include "common.h"
#include "main.exe.h"
#include "gte.h"

/*
 * FUN_80057b80 (0x80057b80, 3796 bytes) — recursive quad/triangle subdivision
 * renderer of the DrawTMD handler family. Compiled-style C using the PsyQ
 * inline-GTE macros (gte_ldv3/gte_rtpt/gte_stsxy3/gte_stsz3) at two RTPT sites.
 *
 * STATUS: NON_MATCHING — LENGTH IS NOW EXACT (945 real instructions, 3796
 * bytes, no knock-on shift to the rest of the image). Residual: 759 differing
 * bytes, dominated by the param_1/param_2 $s0/$s1 swap.
 *
 * The `lwc2; nop; nop; RTPT` shape at both command sites (0x80058320,
 * 0x80058548) is confirmed from the raw image bytes, so the nop-carrying
 * `gte_rtpt()` is correct here (this is a COMPILED caller — docs/gte-policy.md).
 * Note both dumps elide the paired zero words as objdump `...` runs, so the
 * captured-instruction counts (945) each omit 4 real nops; 945+4 = 949 words
 * = 3796 bytes.
 *
 * The body reproduces Ghidra's logic exactly. Two source-structure findings got
 * the register allocation to match almost entirely:
 *   1. local_30 must be computed as an independent `param_1 + 0x22`, NOT copied
 *      from piVar13: the copy carries a register-preference edge that makes cc1
 *      coalesce the two into one register, losing the target's 0x10(sp) spill.
 *   2. Each midpoint block writes its fields THROUGH its own SVECTOR pointer
 *      (r0/r1_00/r2_01/r1/r2_02) after the first store establishes the base —
 *      writing them all as `param_1 + offset` starves the midpoints of refs, so
 *      they score below piVar13 and steal its register.
 * With both, s2..s8 reference counts match the target exactly (20/17/18/13/18/
 * 18/19) and piVar13 correctly lands in $fp.
 *
 *   3. `sVar1` is a SIGNED `short`, not a `u16` (Ghidra had it right). The two
 *      reads of the same halfword — `sVar1 = *(short *)(p + 0xc)` and
 *      `uVar2 = *(u16 *)(p + 0xc)` — are DIFFERENT modes, so cc1 emits two
 *      loads (`lh` + `lhu`) and does not CSE them. Declaring sVar1 as `u16`
 *      collapsed both to one `lhu` plus a redundant `andi rX,rY,0xffff` copy,
 *      and forced the `(short)` compare operands to be sign-extended in-register
 *      with `sll`/`sra` instead of reloaded with `lh`. That cost exactly the two
 *      surplus instructions; fixing the declaration made the length exact.
 *
 * ---------------------------------------------------------------------------
 * ROUND 2 FINDINGS — read before touching this again.
 *
 * The length is exact, but it balances by CANCELLATION (-1 -1 +1 +1), not
 * because every site is right. Align on MNEMONICS ONLY to see the real
 * structure (a plain per-index diff desynchronises after the first
 * insert/delete, and difflib cannot align across the s0/s1 swap). The four
 * open sites, all confirmed against the target asm:
 *
 *   A. MISSING (-1) `addiu a2,param_2,76` at target index 25 (0x80057be0).
 *      PROVEN: the target keeps a base pointer to `param_2 + 0x13` and the
 *      LEAF block reads all three of its fields through it —
 *      `lhu v0,14(a2)`/`lhu v0,26(a2)`/`lw v0,0(a2)` = param_2 + 90/102/76.
 *      The four RECURSIVE blocks instead read 90/102 directly off param_2 in
 *      BOTH builds (identical indices 683/686/757/760/831/834/905/908), so the
 *      pointer is leaf-only. Offsets 76/90/102 all fit a signed 16-bit
 *      immediate, so cc1 would never invent the base — it is source structure.
 *      Adding `int *proto = param_2 + 0x13;` at the top and spelling the leaf's
 *      three reads through it reproduces `addiu a2,s0,76` at EXACTLY index 25
 *      and the leaf reads at 274-282. It costs +1 (-> 946 insns = 3800 bytes),
 *      which OVERFLOWS the 3796-byte carve and makes matchdiff refuse to score,
 *      so it can only land together with a payback from site B/C below.
 *      Bonus: it also drops param_2 from 104 to 102 refs (see D).
 *
 *   B/C. SURPLUS (+1 each) at 0x80057d00 (X box, param_2+44) and 0x80057df4
 *      (Y box, param_2+46). We emit `sll`/`sra` where the target emits one
 *      `lh` reload.
 *   D-adjacent. MISSING (-1) the `lw param_2[7]` reload at 0x80057c14.
 *
 *      A, B and C share ONE root cause, established from the RTL dumps: cc1's
 *      expander emits `movhi` + `ashift` + `ashiftrt` carrying a
 *      `REG_EQUAL (sign_extend:SI (mem:HI ...))` note, and COMBINE normally
 *      folds that triple back into a single `lh`. At the first read after a
 *      store to the same address IN THE SAME BASIC BLOCK, cse's store-to-load
 *      forwarding has already replaced the movhi's source MEM with the stored
 *      register, so combine has no load left to fold and the sll/sra survive.
 *      Later reads (different block, cse table cleared) keep the movhi and DO
 *      become `lh` — see the draft .s, where param_2+44 is read as sll/sra once
 *      and as `lh` twice. The target reloads at ALL of them.
 *      The same forwarding explains the missing `lw param_2[7]` reload.
 *
 *      MECHANISM CONFIRMED, spelling NOT yet found: routing the STORE through
 *      an alias assigned in a different basic block (`int *ctx = param_2;` at
 *      the top, then `ctx[7] = ...`) makes cse unable to prove the addresses
 *      equal and the `lw param_2[7]` reload appears exactly where the target
 *      has it. But the store then uses ctx's own register (`sw v0,28(a1)`)
 *      where the target uses param_2's (`sw v0,28(s1)`) — ctx and param_2 are
 *      both live so they never coalesce. The target performs the store AND the
 *      reload through the same base register, so the real source blocks the
 *      forwarding some other way. Do not ship the ctx alias.
 *
 *   D. The dominant byte residual: param_1/param_2 swapped between $s0/$s1.
 *      Measured with tools/regalloc.py (allocnos identified by the copy-chains
 *      `i4 a0->s1` = p80 = param_1, `i6 a1->s0` = p81 = param_2):
 *          p80 param_1: 74 refs / 1316 live -> 3373
 *          p81 param_2: 104 refs / 1470 live -> 4244
 *      p81 outranks p80, so param_2 takes $s0 first; the target has param_1 in
 *      $s0. `regalloc.py FUN_80057b80 --compare 80 81` says p80 needs +20
 *      weighted refs (+18 with `proto` applied). Ballast alone is a big ask;
 *      note allocno_compare ties break on the LOWER allocno number, and p80 <
 *      p81 — so making the two priorities EQUAL also hands param_1 $s0.
 *
 * Aligned operand residual: 629/945 instructions match as-is; 745/945 would
 * match under a mechanical s0<->s1 rename (the ~6 prologue save-slot lines
 * that rename wrongly flips are position-fixed, not swapped).
 *
 * Do NOT re-run: autorules (46 candidates, no win) or the permuter (refused
 * outright for gte.h functions — its parser cannot read inline asm).
 * Build the draft with `NON_MATCHING=FUN_80057b80 ./Build`.
 */

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80057b80", FUN_80057b80);
#else
void FUN_80057b80(int *param_1, int *param_2, int param_3)
{
    short sVar1;
    u16 uVar2;
    int iVar3;
    u32 *puVar4;
    u32 *puVar5;
    int iVar6;
    short *psVar7;
    long *r2;
    int iVar8;
    short *psVar10;
    long *r2_00;
    SVECTOR *r2_01;
    SVECTOR *r2_02;
    SVECTOR *r1;
    SVECTOR *r0;
    SVECTOR *r1_00;
    int *piVar13;
    int *local_30;

    piVar13 = param_1 + 0x22;
    local_30 = param_1 + 0x22;
    if (*(int *)(*param_1 + 0x10) > *(int *)(param_1[1] + 0x10))
    {
        param_2[6] = *(int *)(*param_1 + 0x10);
        iVar3 = param_1[1];
    }
    else
    {
        param_2[6] = *(int *)(param_1[1] + 0x10);
        iVar3 = *param_1;
    }
    param_2[7] = *(int *)(iVar3 + 0x10);
    iVar3 = *(int *)(param_1[2] + 0x10);
    if (iVar3 < param_2[7])
    {
        param_2[7] = iVar3;
    }
    else if (param_2[6] < iVar3)
    {
        param_2[6] = iVar3;
    }
    iVar3 = *(int *)(param_1[3] + 0x10);
    if (iVar3 < param_2[7])
    {
        param_2[7] = iVar3;
    }
    else if (param_2[6] < iVar3)
    {
        param_2[6] = iVar3;
    }
    iVar3 = param_2[6];
    if (iVar3 < 0)
    {
        iVar3 = iVar3 + 3;
    }
    param_2[6] = iVar3 >> 2;
    if (param_2[8] <= iVar3 >> 2)
    {
        if (*(short *)(*param_1 + 0xc) > *(short *)(param_1[1] + 0xc))
        {
            *(u16 *)(param_2 + 0xc) = *(u16 *)(*param_1 + 0xc);
            iVar3 = param_1[1];
        }
        else
        {
            *(u16 *)(param_2 + 0xc) = *(u16 *)(param_1[1] + 0xc);
            iVar3 = *param_1;
        }
        *(u16 *)(param_2 + 0xb) = *(u16 *)(iVar3 + 0xc);
        sVar1 = *(short *)(param_1[2] + 0xc);
        uVar2 = *(u16 *)(param_1[2] + 0xc);
        if (sVar1 < *(short *)(param_2 + 0xb))
        {
            *(u16 *)(param_2 + 0xb) = uVar2;
        }
        else if (*(short *)(param_2 + 0xc) < sVar1)
        {
            *(u16 *)(param_2 + 0xc) = uVar2;
        }
        sVar1 = *(short *)(param_1[3] + 0xc);
        uVar2 = *(u16 *)(param_1[3] + 0xc);
        if (sVar1 < *(short *)(param_2 + 0xb))
        {
            *(u16 *)(param_2 + 0xb) = uVar2;
        }
        else if (*(short *)(param_2 + 0xc) < sVar1)
        {
            *(u16 *)(param_2 + 0xc) = uVar2;
        }
        if ((-(int)*(short *)(param_2 + 0xd) <= (int)*(short *)(param_2 + 0xc)) &&
            ((int)*(short *)(param_2 + 0xb) <= (int)*(short *)(param_2 + 0xd)))
        {
            if (*(short *)(*param_1 + 0xe) > *(short *)(param_1[1] + 0xe))
            {
                *(u16 *)((int)param_2 + 0x32) = *(u16 *)(*param_1 + 0xe);
                iVar3 = param_1[1];
            }
            else
            {
                *(u16 *)((int)param_2 + 0x32) = *(u16 *)(param_1[1] + 0xe);
                iVar3 = *param_1;
            }
            *(u16 *)((int)param_2 + 0x2e) = *(u16 *)(iVar3 + 0xe);
            sVar1 = *(short *)(param_1[2] + 0xe);
            uVar2 = *(u16 *)(param_1[2] + 0xe);
            if (sVar1 < *(short *)((int)param_2 + 0x2e))
            {
                *(u16 *)((int)param_2 + 0x2e) = uVar2;
            }
            else if (*(short *)((int)param_2 + 0x32) < sVar1)
            {
                *(u16 *)((int)param_2 + 0x32) = uVar2;
            }
            sVar1 = *(short *)(param_1[3] + 0xe);
            uVar2 = *(u16 *)(param_1[3] + 0xe);
            if (sVar1 < *(short *)((int)param_2 + 0x2e))
            {
                *(u16 *)((int)param_2 + 0x2e) = uVar2;
            }
            else if (*(short *)((int)param_2 + 0x32) < sVar1)
            {
                *(u16 *)((int)param_2 + 0x32) = uVar2;
            }
            if ((-(int)*(short *)(param_2 + 0xd) <= (int)*(short *)((int)param_2 + 0x32)) &&
                ((int)*(short *)((int)param_2 + 0x2e) <= (int)*(short *)(param_2 + 0xd)))
            {
                if ((*param_2 == param_3) ||
                    ((*(short *)(param_2 + 0xc) - *(short *)(param_2 + 0xb) < 0xff) &&
                     (*(short *)((int)param_2 + 0x32) - *(short *)((int)param_2 + 0x2e) < 0x7f)))
                {
                    iVar3 = param_2[5];
                    *(u32 *)(iVar3 + 8) = *(u32 *)(*param_1 + 0xc);
                    *(u32 *)(iVar3 + 0x14) = *(u32 *)(param_1[1] + 0xc);
                    *(u32 *)(iVar3 + 0x20) = *(u32 *)(param_1[2] + 0xc);
                    *(u32 *)(iVar3 + 0x2c) = *(u32 *)(param_1[3] + 0xc);
                    *(u32 *)(iVar3 + 0xc) = *(u32 *)(*param_1 + 0x14);
                    *(u32 *)(iVar3 + 0x18) = *(u32 *)(param_1[1] + 0x14);
                    *(u32 *)(iVar3 + 0x24) = *(u32 *)(param_1[2] + 0x14);
                    *(u32 *)(iVar3 + 0x30) = *(u32 *)(param_1[3] + 0x14);
                    *(u32 *)(iVar3 + 4) = *(u32 *)(*param_1 + 8);
                    *(u32 *)(iVar3 + 0x10) = *(u32 *)(param_1[1] + 8);
                    *(u32 *)(iVar3 + 0x1c) = *(u32 *)(param_1[2] + 8);
                    *(u32 *)(iVar3 + 0x28) = *(u32 *)(param_1[3] + 8);
                    *(u16 *)(iVar3 + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    *(u16 *)(iVar3 + 0x1a) = *(u16 *)((int)param_2 + 0x66);
                    *(int *)param_2[5] = param_2[0x13];
                    puVar4 = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar4;
                    *(u32 *)param_2[5] = *puVar4 & 0xffffff | 0xc000000;
                    *(u32 *)param_2[0xe] = param_2[5] & 0xffffff;
                    iVar3 = param_2[5] + 0x34;
                }
                else
                {
                    psVar7 = (short *)*param_1;
                    psVar10 = (short *)param_1[1];
                    *(short *)(param_1 + 4) = (short)(((int)*psVar7 + (int)*psVar10) / 2);
                    r0 = (SVECTOR *)(param_1 + 4);
                    *(short *)((int)r0 + 2) = (short)(((int)psVar7[1] + (int)psVar10[1]) / 2);
                    *(short *)((int)r0 + 4) = (short)(((int)psVar7[2] + (int)psVar10[2]) / 2);
                    *(char *)((int)r0 + 8) = (char)((int)((u32)*(u8 *)(psVar7 + 4) + (u32)*(u8 *)(psVar10 + 4)) >> 1);
                    *(char *)((int)r0 + 9) = (char)((int)((u32)*(u8 *)((int)psVar7 + 9) + (u32)*(u8 *)((int)psVar10 + 9)) >> 1);
                    *(char *)((int)r0 + 0xa) = (char)((int)((u32)*(u8 *)(psVar7 + 5) + (u32)*(u8 *)(psVar10 + 5)) >> 1);
                    *(u8 *)((int)r0 + 0xb) = *(u8 *)((int)psVar7 + 0xb);
                    *(char *)((int)r0 + 0x14) = (char)((int)((u32)*(u8 *)(psVar7 + 10) + (u32)*(u8 *)(psVar10 + 10)) >> 1);
                    *(char *)((int)r0 + 0x15) = (char)((int)((u32)*(u8 *)((int)psVar7 + 0x15) + (u32)*(u8 *)((int)psVar10 + 0x15)) >> 1);
                    psVar7 = (short *)*param_1;
                    psVar10 = (short *)param_1[2];
                    *(short *)(param_1 + 10) = (short)(((int)*psVar7 + (int)*psVar10) / 2);
                    r1_00 = (SVECTOR *)(param_1 + 10);
                    *(short *)((int)r1_00 + 2) = (short)(((int)psVar7[1] + (int)psVar10[1]) / 2);
                    *(short *)((int)r1_00 + 4) = (short)(((int)psVar7[2] + (int)psVar10[2]) / 2);
                    *(char *)((int)r1_00 + 8) = (char)((int)((u32)*(u8 *)(psVar7 + 4) + (u32)*(u8 *)(psVar10 + 4)) >> 1);
                    *(char *)((int)r1_00 + 9) = (char)((int)((u32)*(u8 *)((int)psVar7 + 9) + (u32)*(u8 *)((int)psVar10 + 9)) >> 1);
                    *(char *)((int)r1_00 + 0xa) = (char)((int)((u32)*(u8 *)(psVar7 + 5) + (u32)*(u8 *)(psVar10 + 5)) >> 1);
                    *(u8 *)((int)r1_00 + 0xb) = *(u8 *)((int)psVar7 + 0xb);
                    *(char *)((int)r1_00 + 0x14) = (char)((int)((u32)*(u8 *)(psVar7 + 10) + (u32)*(u8 *)(psVar10 + 10)) >> 1);
                    *(char *)((int)r1_00 + 0x15) = (char)((int)((u32)*(u8 *)((int)psVar7 + 0x15) + (u32)*(u8 *)((int)psVar10 + 0x15)) >> 1);
                    psVar7 = (short *)param_1[2];
                    psVar10 = (short *)param_1[3];
                    *(short *)(param_1 + 0x10) = (short)(((int)*psVar7 + (int)*psVar10) / 2);
                    r2_01 = (SVECTOR *)(param_1 + 0x10);
                    *(short *)((int)r2_01 + 2) = (short)(((int)psVar7[1] + (int)psVar10[1]) / 2);
                    *(short *)((int)r2_01 + 4) = (short)(((int)psVar7[2] + (int)psVar10[2]) / 2);
                    *(char *)((int)r2_01 + 8) = (char)((int)((u32)*(u8 *)(psVar7 + 4) + (u32)*(u8 *)(psVar10 + 4)) >> 1);
                    *(char *)((int)r2_01 + 9) = (char)((int)((u32)*(u8 *)((int)psVar7 + 9) + (u32)*(u8 *)((int)psVar10 + 9)) >> 1);
                    *(char *)((int)r2_01 + 0xa) = (char)((int)((u32)*(u8 *)(psVar7 + 5) + (u32)*(u8 *)(psVar10 + 5)) >> 1);
                    *(u8 *)((int)r2_01 + 0xb) = *(u8 *)((int)psVar7 + 0xb);
                    *(char *)((int)r2_01 + 0x14) = (char)((int)((u32)*(u8 *)(psVar7 + 10) + (u32)*(u8 *)(psVar10 + 10)) >> 1);
                    *(char *)((int)r2_01 + 0x15) = (char)((int)((u32)*(u8 *)((int)psVar7 + 0x15) + (u32)*(u8 *)((int)psVar10 + 0x15)) >> 1);
                    gte_ldv3(r0, r1_00, r2_01);
                    gte_rtpt();
                    psVar7 = (short *)param_1[3];
                    psVar10 = (short *)param_1[1];
                    *(short *)(param_1 + 0x16) = (short)(((int)*psVar7 + (int)*psVar10) / 2);
                    r1 = (SVECTOR *)(param_1 + 0x16);
                    *(short *)((int)r1 + 2) = (short)(((int)psVar7[1] + (int)psVar10[1]) / 2);
                    *(short *)((int)r1 + 4) = (short)(((int)psVar7[2] + (int)psVar10[2]) / 2);
                    *(char *)((int)r1 + 8) = (char)((int)((u32)*(u8 *)(psVar7 + 4) + (u32)*(u8 *)(psVar10 + 4)) >> 1);
                    *(char *)((int)r1 + 9) = (char)((int)((u32)*(u8 *)((int)psVar7 + 9) + (u32)*(u8 *)((int)psVar10 + 9)) >> 1);
                    *(char *)((int)r1 + 0xa) = (char)((int)((u32)*(u8 *)(psVar7 + 5) + (u32)*(u8 *)(psVar10 + 5)) >> 1);
                    *(u8 *)((int)r1 + 0xb) = *(u8 *)((int)psVar7 + 0xb);
                    *(char *)((int)r1 + 0x14) = (char)((int)((u32)*(u8 *)(psVar7 + 10) + (u32)*(u8 *)(psVar10 + 10)) >> 1);
                    *(char *)((int)r1 + 0x15) = (char)((int)((u32)*(u8 *)((int)psVar7 + 0x15) + (u32)*(u8 *)((int)psVar10 + 0x15)) >> 1);
                    psVar7 = (short *)*param_1;
                    psVar10 = (short *)param_1[3];
                    *(short *)(param_1 + 0x1c) = (short)(((int)*psVar7 + (int)*psVar10) / 2);
                    r2_02 = (SVECTOR *)(param_1 + 0x1c);
                    *(short *)((int)r2_02 + 2) = (short)(((int)psVar7[1] + (int)psVar10[1]) / 2);
                    *(short *)((int)r2_02 + 4) = (short)(((int)psVar7[2] + (int)psVar10[2]) / 2);
                    *(char *)((int)r2_02 + 8) = (char)((int)((u32)*(u8 *)(psVar7 + 4) + (u32)*(u8 *)(psVar10 + 4)) >> 1);
                    *(char *)((int)r2_02 + 9) = (char)((int)((u32)*(u8 *)((int)psVar7 + 9) + (u32)*(u8 *)((int)psVar10 + 9)) >> 1);
                    *(char *)((int)r2_02 + 0xa) = (char)((int)((u32)*(u8 *)(psVar7 + 5) + (u32)*(u8 *)(psVar10 + 5)) >> 1);
                    *(u8 *)((int)r2_02 + 0xb) = *(u8 *)((int)psVar7 + 0xb);
                    *(char *)((int)r2_02 + 0x14) = (char)((int)((u32)*(u8 *)(psVar7 + 10) + (u32)*(u8 *)(psVar10 + 10)) >> 1);
                    r2_00 = param_1 + 0x13;
                    *(char *)((int)r2_02 + 0x15) = (char)((int)((u32)*(u8 *)((int)psVar7 + 0x15) + (u32)*(u8 *)((int)psVar10 + 0x15)) >> 1);
                    gte_stsxy3(param_1 + 7, param_1 + 0xd, r2_00);
                    r2 = param_1 + 0x14;
                    gte_stsz3(param_1 + 8, param_1 + 0xe, r2);
                    gte_ldv3(r2_01, r1, r2_02);
                    gte_rtpt();
                    iVar3 = *param_1;
                    piVar13[1] = (int)r0;
                    piVar13[2] = (int)r1_00;
                    piVar13[3] = (int)r2_02;
                    *piVar13 = iVar3;
                    gte_stsxy3(r2_00, param_1 + 0x19, param_1 + 0x1f);
                    gte_stsz3(r2, param_1 + 0x1a, param_1 + 0x20);
                    param_3 = param_3 + 1;
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar6 = *param_1;
                    puVar4 = (u32 *)param_2[5];
                    iVar8 = param_1[1];
                    puVar4[2] = *(u32 *)(iVar6 + 0xc);
                    puVar4[5] = *(u32 *)(iVar8 + 0xc);
                    puVar4[8] = *(u32 *)&r0[1].vz;
                    iVar3 = *(int *)(iVar6 + 0x10);
                    if (iVar3 < 0)
                    {
                        iVar3 = iVar3 + 3;
                    }
                    param_2[6] = iVar3 >> 2;
                    puVar4[3] = (u32)*(u16 *)(iVar6 + 0x14);
                    puVar4[6] = (u32)*(u16 *)(iVar8 + 0x14);
                    puVar4[9] = (u32)(u16)r0[2].vz;
                    puVar4[1] = *(u32 *)(iVar6 + 8);
                    puVar4[4] = *(u32 *)(iVar8 + 8);
                    puVar4[7] = *(u32 *)(r0 + 1);
                    *(u16 *)((int)puVar4 + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2 = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4 + 3) = 9;
                    *(u8 *)((int)puVar4 + 7) = 0x34;
                    *(u16 *)((int)puVar4 + 0x1a) = uVar2;
                    puVar5 = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5;
                    *puVar4 = *puVar5 & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4 & 0xffffff;
                    param_2[5] = param_2[5] + 0x28;
                    *piVar13 = (int)r0;
                    iVar3 = param_1[1];
                    piVar13[2] = (int)r2_02;
                    piVar13[1] = iVar3;
                    piVar13[3] = (int)r1;
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar6 = param_1[2];
                    puVar4 = (u32 *)param_2[5];
                    iVar8 = *param_1;
                    puVar4[2] = *(u32 *)(iVar6 + 0xc);
                    puVar4[5] = *(u32 *)(iVar8 + 0xc);
                    puVar4[8] = *(u32 *)&r1_00[1].vz;
                    iVar3 = *(int *)(iVar6 + 0x10);
                    if (iVar3 < 0)
                    {
                        iVar3 = iVar3 + 3;
                    }
                    param_2[6] = iVar3 >> 2;
                    puVar4[3] = (u32)*(u16 *)(iVar6 + 0x14);
                    puVar4[6] = (u32)*(u16 *)(iVar8 + 0x14);
                    puVar4[9] = (u32)(u16)r1_00[2].vz;
                    puVar4[1] = *(u32 *)(iVar6 + 8);
                    puVar4[4] = *(u32 *)(iVar8 + 8);
                    puVar4[7] = *(u32 *)(r1_00 + 1);
                    *(u16 *)((int)puVar4 + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2 = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4 + 3) = 9;
                    *(u8 *)((int)puVar4 + 7) = 0x34;
                    *(u16 *)((int)puVar4 + 0x1a) = uVar2;
                    puVar5 = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5;
                    *puVar4 = *puVar5 & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4 & 0xffffff;
                    param_2[5] = param_2[5] + 0x28;
                    *piVar13 = (int)r1_00;
                    piVar13[1] = (int)r2_02;
                    piVar13[2] = param_1[2];
                    piVar13[3] = (int)r2_01;
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar6 = param_1[3];
                    puVar4 = (u32 *)param_2[5];
                    iVar8 = param_1[2];
                    puVar4[2] = *(u32 *)(iVar6 + 0xc);
                    puVar4[5] = *(u32 *)(iVar8 + 0xc);
                    puVar4[8] = *(u32 *)&r2_01[1].vz;
                    iVar3 = *(int *)(iVar6 + 0x10);
                    if (iVar3 < 0)
                    {
                        iVar3 = iVar3 + 3;
                    }
                    param_2[6] = iVar3 >> 2;
                    puVar4[3] = (u32)*(u16 *)(iVar6 + 0x14);
                    puVar4[6] = (u32)*(u16 *)(iVar8 + 0x14);
                    puVar4[9] = (u32)(u16)r2_01[2].vz;
                    puVar4[1] = *(u32 *)(iVar6 + 8);
                    puVar4[4] = *(u32 *)(iVar8 + 8);
                    puVar4[7] = *(u32 *)(r2_01 + 1);
                    *(u16 *)((int)puVar4 + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2 = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4 + 3) = 9;
                    *(u8 *)((int)puVar4 + 7) = 0x34;
                    *(u16 *)((int)puVar4 + 0x1a) = uVar2;
                    puVar5 = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5;
                    *puVar4 = *puVar5 & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4 & 0xffffff;
                    param_2[5] = param_2[5] + 0x28;
                    *piVar13 = (int)r2_02;
                    piVar13[1] = (int)r1;
                    piVar13[2] = (int)r2_01;
                    piVar13[3] = param_1[3];
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar8 = param_1[1];
                    puVar4 = (u32 *)param_2[5];
                    iVar6 = param_1[3];
                    puVar4[2] = *(u32 *)(iVar8 + 0xc);
                    puVar4[5] = *(u32 *)(iVar6 + 0xc);
                    puVar4[8] = *(u32 *)&r1[1].vz;
                    iVar3 = *(int *)(iVar8 + 0x10);
                    if (iVar3 < 0)
                    {
                        iVar3 = iVar3 + 3;
                    }
                    param_2[6] = iVar3 >> 2;
                    puVar4[3] = (u32)*(u16 *)(iVar8 + 0x14);
                    puVar4[6] = (u32)*(u16 *)(iVar6 + 0x14);
                    puVar4[9] = (u32)(u16)r1[2].vz;
                    puVar4[1] = *(u32 *)(iVar8 + 8);
                    puVar4[4] = *(u32 *)(iVar6 + 8);
                    puVar4[7] = *(u32 *)(r1 + 1);
                    *(u16 *)((int)puVar4 + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2 = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4 + 3) = 9;
                    *(u8 *)((int)puVar4 + 7) = 0x34;
                    *(u16 *)((int)puVar4 + 0x1a) = uVar2;
                    puVar5 = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5;
                    *puVar4 = *puVar5 & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4 & 0xffffff;
                    iVar3 = param_2[5] + 0x28;
                }
                param_2[5] = iVar3;
            }
        }
    }
    return;
}
#endif
