#include "common.h"
#include "main.exe.h"
#include "gte.h"

/*
 * FUN_80057b80 (0x80057b80, 3796 bytes) — recursive quad/triangle subdivision
 * renderer of the DrawTMD handler family. Compiled-style C using the PsyQ
 * inline-GTE macros (gte_ldv3/gte_rtpt/gte_stsxy3/gte_stsz3) at two RTPT sites.
 *
 * STATUS: NON_MATCHING — first full reconstruction of this 3796-byte function.
 * The draft links 3804 bytes: 947 real instructions where the target has 945,
 * i.e. TWO SURPLUS INSTRUCTIONS to find and remove (asmdiff -n --structural).
 *
 * READ THIS BEFORE TRUSTING AN OLDER NUMBER: the worker measured "3788, 2
 * instructions SHORT" against a private gte.h whose command macros had no
 * latency nops. That arithmetic is void. The target provably issues
 * `lwc2; nop; nop; RTPT` at both command sites (0x80058320, 0x80058548), so the
 * nop-carrying `gte_rtpt()` is correct here (this is a COMPILED caller — see
 * docs/gte-policy.md); with it the four nops land and the draft is 2 real
 * instructions OVER, not under. Chase the surplus, not a shortfall.
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
 * 18/19) and piVar13 correctly lands in $fp. The residual: param_1/param_2 are
 * swapped between $s0/$s1 (cc1 scores param_2 above param_1 here), and the
 * `move s8,t0` sinks out of the prologue. Build the draft with
 * `NON_MATCHING=FUN_80057b80 ./Build`.
 */

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80057b80", FUN_80057b80);
#else
void FUN_80057b80(int *param_1, int *param_2, int param_3)
{
    u16 sVar1;
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
