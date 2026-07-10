#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80033f10", FUN_80033f10);

// triage: MEDIUM — 193 insns, mul/div, 5 callees, ~0.04 to ReqItemShinsoku
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Removing unreachable block (ram,0x80034050) */
// /* WARNING: Removing unreachable block (ram,0x80034010) */
// /* WARNING: Removing unreachable block (ram,0x80034090) */
// /* DrawImpact? */
//
// void FUN_80033f10(undefined4 *param_1)
//
// {
//   byte bVar1;
//   short sVar2;
//   int iVar3;
//   long lVar4;
//   uint uVar5;
//   int iVar6;
//   int iVar7;
//   int iVar8;
//   int iVar9;
//   uint uVar10;
//   undefined4 local_20;
//   ushort local_1c;
//
//   uVar5 = (uint)*(byte *)((int)param_1 + 0x26);
//   uVar10 = ((uint)*(byte *)((int)param_1 + 0x25) << 0xc) / uVar5;
//   if (uVar5 == 0) {
//     trap(0x1c00);
//   }
//   if ((uVar5 == 0xffffffff) && (*(byte *)((int)param_1 + 0x25) == 0x80000)) {
//     trap(0x1800);
//   }
//   bVar1 = *(byte *)(param_1 + 9);
//   iVar6 = (uint)bVar1 * 0x24;
//   *(int *)(&DAT_800beac8 + iVar6) = (int)*(short *)(param_1 + 8) << 0xc;
//   iVar9 = 0x1000 - uVar10;
//   iVar7 = *(short *)(param_1 + 7) * iVar9;
//   *(short *)(param_1 + 8) = *(short *)(param_1 + 8) + *(short *)((int)param_1 + 0x22);
//   if (iVar7 < 0) {
//     iVar7 = iVar7 + 0xfff;
//   }
//   iVar3 = (int)*(short *)((int)param_1 + 0x1e) * uVar10;
//   if (iVar3 < 0) {
//     iVar3 = iVar3 + 0xfff;
//   }
//   iVar8 = (uint)*(byte *)((int)param_1 + 0x16) * iVar9;
//   if (iVar8 < 0) {
//     iVar8 = iVar8 + 0xfff;
//   }
//   (&DAT_800beabc)[iVar6] =
//        (char)(iVar8 >> 0xc) + (char)((int)(*(byte *)((int)param_1 + 0x1a) * uVar10) >> 0xc);
//   iVar8 = (uint)*(byte *)((int)param_1 + 0x15) * iVar9;
//   if (iVar8 < 0) {
//     iVar8 = iVar8 + 0xfff;
//   }
//   (&DAT_800beabd)[iVar6] =
//        (char)(iVar8 >> 0xc) + (char)((int)(*(byte *)((int)param_1 + 0x19) * uVar10) >> 0xc);
//   iVar9 = (uint)*(byte *)(param_1 + 5) * iVar9;
//   if (iVar9 < 0) {
//     iVar9 = iVar9 + 0xfff;
//   }
//   (&DAT_800beabe)[iVar6] =
//        (char)(iVar9 >> 0xc) + (char)((int)(*(byte *)(param_1 + 6) * uVar10) >> 0xc);
//   if ((GsCOORDINATE2 *)param_1[4] == (GsCOORDINATE2 *)0x0) {
//     FUN_800396c0(param_1[1],param_1[2],param_1[3],&local_20);
//   }
//   else {
//     Scratchpad._32_2_ = (undefined2)param_1[1];
//     Scratchpad._34_2_ = (undefined2)param_1[2];
//     Scratchpad._36_2_ = (undefined2)param_1[3];
//     GsGetLs((GsCOORDINATE2 *)param_1[4],(MATRIX *)Scratchpad);
//     GsSetLsMatrix((MATRIX *)Scratchpad);
//     lVar4 = RotTransPers((SVECTOR *)(Scratchpad + 0x20),&local_20,(long *)(Scratchpad + 0x28),
//                          (long *)(Scratchpad + 0x2c));
//     local_1c = (ushort)lVar4;
//   }
//   iVar9 = (int)(short)local_1c;
//   if (0x24 < iVar9) {
//     iVar7 = ((iVar7 >> 0xc) + (iVar3 >> 0xc)) * 300;
//     if (iVar9 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar9 == -1) && (iVar7 == -0x80000000)) {
//       trap(0x1800);
//     }
//     sVar2 = (short)(iVar7 / iVar9) + 1;
//     *(short *)(&DAT_800beac6 + iVar6) = sVar2;
//     *(short *)(&DAT_800beac4 + iVar6) = sVar2;
//     *(undefined2 *)(&DAT_800beaac + iVar6) = (undefined2)local_20;
//     *(undefined2 *)(&DAT_800beaae + iVar6) = local_20._2_2_;
//     iVar7 = (int)((uint)local_1c << 0x10) >> 0x12;
//     if (iVar7 < 0) {
//       iVar9 = 0;
//     }
//     else {
//       iVar9 = 0x4e1;
//       if (iVar7 < 0x4e2) {
//         iVar9 = iVar7;
//       }
//     }
//     GsSortSprite((GsSPRITE *)(&DAT_800beaa8 + (uint)bVar1 * 9),OTablePt,(ushort)iVar9);
//   }
//   if (*(byte *)((int)param_1 + 0x26) <= *(byte *)((int)param_1 + 0x25)) {
//     *param_1 = 0;
//   }
//   *(char *)((int)param_1 + 0x25) = *(char *)((int)param_1 + 0x25) + '\x01';
//   return;
// }
