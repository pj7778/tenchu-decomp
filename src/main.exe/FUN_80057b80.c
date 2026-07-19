#include "common.h"
#include "main.exe.h"
#include "gte.h"
/*
 * MATCHED: recursive primitive subdivision renderer using the PsyQ inline-GTE
 * macros.  The two pointer arguments are first copied into ordinary locals,
 * with the second assignment written before the first.  gcc coalesces those
 * locals into s1 and s0 while retaining that source order, which produces the
 * target's s1/a1 save-copy pair before its s0/a0 pair.
 *
 * This disproves the former signature-only impossibility proof: it assumed
 * every body use had to remain on the formal-parameter pseudos.  The nested
 * do/while boundary around the leaf vertex copies remains allocation-relevant;
 * it is consistent with nested primitive-copy macro expansion, while removing
 * it changes the function-wide register assignment.
 */

void FUN_80057b80(int *arg_1, int *arg_2, int param_3)
{
    int *param_1;
    int *param_2;
    short sVar1;
    u16 uVar2;
    u16 uVar2_a;
    u16 uVar2_b;
    u16 uVar2_c;
    u16 uVar2_d;
    int iVar3;
    int zA;
    int zB;
    int zC;
    int prim;
    int pv;
    int pv2;
    int dz1;
    int dz2;
    int dz3;
    int dz4;
    u32 *otp;
    u32 *puVar4_a;
    u32 *puVar4_b;
    u32 *puVar4_c;
    u32 *puVar4_d;
    u32 *puVar5_a;
    u32 *puVar5_b;
    u32 *puVar5_c;
    u32 *puVar5_d;
    int iVar6_a;
    int iVar6_b;
    int iVar6_c;
    int iVar6_d;
    short *psVar7;
    int iVar8_a;
    int iVar8_b;
    int iVar8_c;
    int iVar8_d;
    short *psVar10;
    long *r2_00;
    SVECTOR *r2_01;
    SVECTOR *r2_02;
    SVECTOR *r1;
    SVECTOR *r0;
    SVECTOR *r1_00;
    int *piVar13;
    int *local_30;
    int *proto;

    param_2 = arg_2;
    param_1 = arg_1;
    piVar13 = param_1 + 0x22;
    local_30 = param_1 + 0x22;
    proto = param_2 + 0x13;
    if (*(int *)(*param_1 + 0x10) > *(int *)(param_1[1] + 0x10))
    {
        param_2[6] = *(int *)(*param_1 + 0x10);
        param_2[7] = *(int *)(param_1[1] + 0x10);
    }
    else
    {
        param_2[6] = *(int *)(param_1[1] + 0x10);
        param_2[7] = *(int *)(*param_1 + 0x10);
    }
    zA = *(int *)(param_1[2] + 0x10);
    if (zA < param_2[7])
    {
        param_2[7] = zA;
    }
    else if (param_2[6] < zA)
    {
        param_2[6] = zA;
    }
    zB = *(int *)(param_1[3] + 0x10);
    if (zB < param_2[7])
    {
        param_2[7] = zB;
    }
    else if (param_2[6] < zB)
    {
        param_2[6] = zB;
    }
    zC = param_2[6];
    if (zC < 0)
    {
        zC = zC + 3;
    }
    param_2[6] = zC >> 2;
    if (param_2[8] <= zC >> 2)
    {
        if (*(short *)(*param_1 + 0xc) > *(short *)(param_1[1] + 0xc))
        {
            *(u16 *)(param_2 + 0xc) = *(u16 *)(*param_1 + 0xc);
            *(u16 *)(param_2 + 0xb) = *(u16 *)(param_1[1] + 0xc);
        }
        else
        {
            *(u16 *)(param_2 + 0xc) = *(u16 *)(param_1[1] + 0xc);
            *(u16 *)(param_2 + 0xb) = *(u16 *)(*param_1 + 0xc);
        }
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
                *(u16 *)((int)param_2 + 0x2e) = *(u16 *)(param_1[1] + 0xe);
            }
            else
            {
                *(u16 *)((int)param_2 + 0x32) = *(u16 *)(param_1[1] + 0xe);
                *(u16 *)((int)param_2 + 0x2e) = *(u16 *)(*param_1 + 0xe);
            }
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
                    do { do { do {
                    prim = param_2[5];
                    *(u32 *)(prim + 8) = *(u32 *)(*param_1 + 0xc);
                    *(u32 *)(prim + 0x14) = *(u32 *)(param_1[1] + 0xc);
                    *(u32 *)(prim + 0x20) = *(u32 *)(param_1[2] + 0xc);
                    *(u32 *)(prim + 0x2c) = *(u32 *)(param_1[3] + 0xc);
                    *(u32 *)(prim + 0xc) = *(u32 *)(*param_1 + 0x14);
                    *(u32 *)(prim + 0x18) = *(u32 *)(param_1[1] + 0x14);
                    *(u32 *)(prim + 0x24) = *(u32 *)(param_1[2] + 0x14);
                    *(u32 *)(prim + 0x30) = *(u32 *)(param_1[3] + 0x14);
                    *(u32 *)(prim + 4) = *(u32 *)(*param_1 + 8);
                    *(u32 *)(prim + 0x10) = *(u32 *)(param_1[1] + 8);
                    *(u32 *)(prim + 0x1c) = *(u32 *)(param_1[2] + 8);
                    *(u32 *)(prim + 0x28) = *(u32 *)(param_1[3] + 8);
                    } while (0); } while (0); } while (0);
                    *(u16 *)(prim + 0xe) = *(u16 *)((int)proto + 0xe);
                    *(u16 *)(prim + 0x1a) = *(u16 *)((int)proto + 0x1a);
                    *(int *)param_2[5] = *proto;
                    otp = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)otp;
                    *(u32 *)param_2[5] = *otp & 0xffffff | 0xc000000;
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
                    gte_stsz3(param_1 + 8, param_1 + 0xe, param_1 + 0x14);
                    gte_ldv3(r2_01, r1, r2_02);
                    gte_rtpt();
                    pv = *param_1;
                    piVar13[1] = (int)r0;
                    piVar13[2] = (int)r1_00;
                    piVar13[3] = (int)r2_02;
                    *piVar13 = pv;
                    gte_stsxy3(r2_00, param_1 + 0x19, param_1 + 0x1f);
                    gte_stsz3(param_1 + 0x14, param_1 + 0x1a, param_1 + 0x20);
                    param_3 = param_3 + 1;
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar6_a = *param_1;
                    puVar4_a = (u32 *)param_2[5];
                    iVar8_a = param_1[1];
                    puVar4_a[2] = *(u32 *)(iVar6_a + 0xc);
                    puVar4_a[5] = *(u32 *)(iVar8_a + 0xc);
                    puVar4_a[8] = *(u32 *)&r0[1].vz;
                    dz1 = *(int *)(iVar6_a + 0x10);
                    if (dz1 < 0)
                    {
                        dz1 = dz1 + 3;
                    }
                    param_2[6] = dz1 >> 2;
                    puVar4_a[3] = (u32)*(u16 *)(iVar6_a + 0x14);
                    puVar4_a[6] = (u32)*(u16 *)(iVar8_a + 0x14);
                    puVar4_a[9] = (u32)(u16)r0[2].vz;
                    puVar4_a[1] = *(u32 *)(iVar6_a + 8);
                    puVar4_a[4] = *(u32 *)(iVar8_a + 8);
                    puVar4_a[7] = *(u32 *)(r0 + 1);
                    *(u16 *)((int)puVar4_a + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2_a = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4_a + 3) = 9;
                    *(u8 *)((int)puVar4_a + 7) = 0x34;
                    *(u16 *)((int)puVar4_a + 0x1a) = uVar2_a;
                    puVar5_a = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5_a;
                    *puVar4_a = *puVar5_a & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4_a & 0xffffff;
                    param_2[5] = param_2[5] + 0x28;
                    *piVar13 = (int)r0;
                    pv2 = param_1[1];
                    piVar13[2] = (int)r2_02;
                    piVar13[1] = pv2;
                    piVar13[3] = (int)r1;
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar6_b = param_1[2];
                    puVar4_b = (u32 *)param_2[5];
                    iVar8_b = *param_1;
                    puVar4_b[2] = *(u32 *)(iVar6_b + 0xc);
                    puVar4_b[5] = *(u32 *)(iVar8_b + 0xc);
                    puVar4_b[8] = *(u32 *)&r1_00[1].vz;
                    dz2 = *(int *)(iVar6_b + 0x10);
                    if (dz2 < 0)
                    {
                        dz2 = dz2 + 3;
                    }
                    param_2[6] = dz2 >> 2;
                    puVar4_b[3] = (u32)*(u16 *)(iVar6_b + 0x14);
                    puVar4_b[6] = (u32)*(u16 *)(iVar8_b + 0x14);
                    puVar4_b[9] = (u32)(u16)r1_00[2].vz;
                    puVar4_b[1] = *(u32 *)(iVar6_b + 8);
                    puVar4_b[4] = *(u32 *)(iVar8_b + 8);
                    puVar4_b[7] = *(u32 *)(r1_00 + 1);
                    *(u16 *)((int)puVar4_b + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2_b = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4_b + 3) = 9;
                    *(u8 *)((int)puVar4_b + 7) = 0x34;
                    *(u16 *)((int)puVar4_b + 0x1a) = uVar2_b;
                    puVar5_b = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5_b;
                    *puVar4_b = *puVar5_b & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4_b & 0xffffff;
                    param_2[5] = param_2[5] + 0x28;
                    *piVar13 = (int)r1_00;
                    piVar13[1] = (int)r2_02;
                    piVar13[2] = param_1[2];
                    piVar13[3] = (int)r2_01;
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar6_c = param_1[3];
                    puVar4_c = (u32 *)param_2[5];
                    iVar8_c = param_1[2];
                    puVar4_c[2] = *(u32 *)(iVar6_c + 0xc);
                    puVar4_c[5] = *(u32 *)(iVar8_c + 0xc);
                    puVar4_c[8] = *(u32 *)&r2_01[1].vz;
                    dz3 = *(int *)(iVar6_c + 0x10);
                    if (dz3 < 0)
                    {
                        dz3 = dz3 + 3;
                    }
                    param_2[6] = dz3 >> 2;
                    puVar4_c[3] = (u32)*(u16 *)(iVar6_c + 0x14);
                    puVar4_c[6] = (u32)*(u16 *)(iVar8_c + 0x14);
                    puVar4_c[9] = (u32)(u16)r2_01[2].vz;
                    puVar4_c[1] = *(u32 *)(iVar6_c + 8);
                    puVar4_c[4] = *(u32 *)(iVar8_c + 8);
                    puVar4_c[7] = *(u32 *)(r2_01 + 1);
                    *(u16 *)((int)puVar4_c + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2_c = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4_c + 3) = 9;
                    *(u8 *)((int)puVar4_c + 7) = 0x34;
                    *(u16 *)((int)puVar4_c + 0x1a) = uVar2_c;
                    puVar5_c = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5_c;
                    *puVar4_c = *puVar5_c & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4_c & 0xffffff;
                    param_2[5] = param_2[5] + 0x28;
                    *piVar13 = (int)r2_02;
                    piVar13[1] = (int)r1;
                    piVar13[2] = (int)r2_01;
                    piVar13[3] = param_1[3];
                    FUN_80057b80(local_30, param_2, param_3);
                    iVar8_d = param_1[1];
                    puVar4_d = (u32 *)param_2[5];
                    iVar6_d = param_1[3];
                    puVar4_d[2] = *(u32 *)(iVar8_d + 0xc);
                    puVar4_d[5] = *(u32 *)(iVar6_d + 0xc);
                    puVar4_d[8] = *(u32 *)&r1[1].vz;
                    dz4 = *(int *)(iVar8_d + 0x10);
                    if (dz4 < 0)
                    {
                        dz4 = dz4 + 3;
                    }
                    param_2[6] = dz4 >> 2;
                    puVar4_d[3] = (u32)*(u16 *)(iVar8_d + 0x14);
                    puVar4_d[6] = (u32)*(u16 *)(iVar6_d + 0x14);
                    puVar4_d[9] = (u32)(u16)r1[2].vz;
                    puVar4_d[1] = *(u32 *)(iVar8_d + 8);
                    puVar4_d[4] = *(u32 *)(iVar6_d + 8);
                    puVar4_d[7] = *(u32 *)(r1 + 1);
                    *(u16 *)((int)puVar4_d + 0xe) = *(u16 *)((int)param_2 + 0x5a);
                    uVar2_d = *(u16 *)((int)param_2 + 0x66);
                    *(u8 *)((int)puVar4_d + 3) = 9;
                    *(u8 *)((int)puVar4_d + 7) = 0x34;
                    *(u16 *)((int)puVar4_d + 0x1a) = uVar2_d;
                    puVar5_d = (u32 *)(param_2[4] + (param_2[6] >> param_2[3]) * 4);
                    param_2[0xe] = (int)puVar5_d;
                    *puVar4_d = *puVar5_d & 0xffffff | 0x9000000;
                    *(u32 *)param_2[0xe] = (u32)puVar4_d & 0xffffff;
                    iVar3 = param_2[5] + 0x28;
                }
                param_2[5] = iVar3;
            }
        }
    }
    return;
}
