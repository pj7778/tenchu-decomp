#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetWire", SetWire);

// triage: HARD — 372 insns, mul/div, 9 callees, ~0.10 to GetVectorRotation
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetWire(VECTOR *start,VECTOR *end,VECTOR *center,long len)
//
// {
//   bool bVar1;
//   ModelType *dim;
//   long lVar2;
//   int iVar3;
//   int iVar4;
//   int iVar5;
//   int iVar6;
//   int iVar7;
//   int iVar8;
//   int iVar9;
//   int iVar10;
//   long unaff_s7;
//   long unaff_s8;
//   VECTOR local_68;
//   undefined4 local_58;
//   undefined4 local_54;
//   undefined4 local_50;
//   undefined4 local_4c;
//   GsLINE local_48;
//   long local_38;
//   long local_34;
//   long local_30;
//
//   Scratchpad[0x14] = 0;
//   Scratchpad[0x15] = 0;
//   Scratchpad[0x16] = 0;
//   Scratchpad[0x17] = 0;
//   Scratchpad[0x18] = 0;
//   Scratchpad[0x19] = 0;
//   Scratchpad[0x1a] = 0;
//   Scratchpad[0x1b] = 0;
//   Scratchpad[0x1c] = 0;
//   Scratchpad[0x1d] = 0;
//   Scratchpad[0x1e] = 0;
//   Scratchpad[0x1f] = 0;
//   Scratchpad._32_2_ = (short)start->vx - (short)ViewInfo.vpx;
//   Scratchpad._34_2_ = (short)start->vy - (short)ViewInfo.vpy;
//   Scratchpad._36_2_ = (short)start->vz - (short)ViewInfo.vpz;
//   SetTransMatrix((MATRIX *)Scratchpad);
//   SetRotMatrix(&GsWSMATRIX);
//   lVar2 = RotTransPers((SVECTOR *)(Scratchpad + 0x20),&local_50,(long *)(Scratchpad + 0x28),
//                        (long *)(Scratchpad + 0x2c));
//   local_4c = CONCAT22(local_4c._2_2_,(short)lVar2);
//   local_48.r = 'P';
//   local_48.g = 'H';
//   bVar1 = false;
//   local_48.attribute = 0;
//   local_48.b = '8';
//   iVar10 = start->vx - end->vx;
//   iVar8 = start->vy - end->vy;
//   iVar9 = start->vz - end->vz;
//   iVar3 = abs(iVar10);
//   if (((0x1000 < iVar3) || (iVar3 = abs(iVar8), 0x1000 < iVar3)) ||
//      (iVar3 = abs(iVar9), 0x1000 < iVar3)) {
//     bVar1 = true;
//   }
//   if (bVar1) {
//     if (iVar10 < 0) {
//       iVar10 = iVar10 + 0xff;
//     }
//     if (iVar8 < 0) {
//       iVar8 = iVar8 + 0xff;
//     }
//     if (iVar9 < 0) {
//       iVar9 = iVar9 + 0xff;
//     }
//     lVar2 = SquareRoot0((iVar10 >> 8) * (iVar10 >> 8) + (iVar8 >> 8) * (iVar8 >> 8) +
//                         (iVar9 >> 8) * (iVar9 >> 8));
//     lVar2 = lVar2 << 8;
//   }
//   else {
//     lVar2 = SquareRoot0(iVar10 * iVar10 + iVar8 * iVar8 + iVar9 * iVar9);
//   }
//   iVar3 = lVar2 / 300;
//   if (center == (VECTOR *)0x0) {
//     local_68.vx = (end->vx + start->vx) / 2;
//     center = &local_68;
//     if (lVar2 < 0) {
//       lVar2 = lVar2 + 0x1f;
//     }
//     local_68.vy = (end->vy + start->vy) / 2 + (lVar2 >> 5);
//     local_68.vz = (end->vz + start->vz) / 2;
//   }
//   iVar10 = iVar3 * len;
//   if (iVar10 < 0) {
//     iVar10 = iVar10 + 0xfff;
//   }
//   for (iVar8 = 0; iVar8 < iVar10 >> 0xc; iVar8 = iVar8 + 1) {
//     if (iVar3 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar3 == -1) && (iVar8 << 0xc == -0x80000000)) {
//       trap(0x1800);
//     }
//     iVar4 = 0x1000 - (iVar8 << 0xc) / iVar3;
//     iVar9 = iVar4 * iVar4;
//     if (iVar9 < 0) {
//       iVar9 = iVar9 + 0xfff;
//     }
//     iVar9 = iVar9 >> 0xc;
//     iVar7 = iVar4 * -2 + 0x1000 + iVar9;
//     iVar6 = iVar4 * 2 + iVar9 * -2;
//     iVar4 = iVar7 * end->vx + iVar6 * center->vx + iVar9 * start->vx;
//     if (iVar4 < 0) {
//       iVar4 = iVar4 + 0xfff;
//     }
//     iVar5 = iVar7 * end->vy + iVar6 * center->vy + iVar9 * start->vy;
//     local_30 = iVar4 >> 0xc;
//     if (iVar5 < 0) {
//       iVar5 = iVar5 + 0xfff;
//     }
//     iVar9 = iVar7 * end->vz + iVar6 * center->vz + iVar9 * start->vz;
//     unaff_s8 = iVar5 >> 0xc;
//     if (iVar9 < 0) {
//       iVar9 = iVar9 + 0xfff;
//     }
//     Scratchpad[0x14] = 0;
//     Scratchpad[0x15] = 0;
//     Scratchpad[0x16] = 0;
//     Scratchpad[0x17] = 0;
//     Scratchpad[0x18] = 0;
//     Scratchpad[0x19] = 0;
//     Scratchpad[0x1a] = 0;
//     Scratchpad[0x1b] = 0;
//     Scratchpad[0x1c] = 0;
//     Scratchpad[0x1d] = 0;
//     Scratchpad[0x1e] = 0;
//     Scratchpad[0x1f] = 0;
//     Scratchpad._32_2_ = (short)(iVar4 >> 0xc) - (short)ViewInfo.vpx;
//     Scratchpad._34_2_ = (short)unaff_s8 - (short)ViewInfo.vpy;
//     unaff_s7 = iVar9 >> 0xc;
//     Scratchpad._36_2_ = (short)unaff_s7 - (short)ViewInfo.vpz;
//     SetTransMatrix((MATRIX *)Scratchpad);
//     SetRotMatrix(&GsWSMATRIX);
//     lVar2 = RotTransPers((SVECTOR *)(Scratchpad + 0x20),&local_58,(long *)(Scratchpad + 0x28),
//                          (long *)(Scratchpad + 0x2c));
//     local_54 = CONCAT22(local_54._2_2_,(short)lVar2);
//     if ((0 < lVar2 << 0x10) && (0 < (short)local_4c)) {
//       local_48.y0 = local_50._2_2_;
//       iVar9 = (lVar2 << 0x10) >> 0x12;
//       local_48.x0 = (short)local_50;
//       local_48.x1 = (short)local_58;
//       local_48.y1 = local_58._2_2_;
//       if (iVar9 < 0) {
//         iVar4 = 0;
//       }
//       else {
//         iVar4 = 0x4e1;
//         if (iVar9 < 0x4e2) {
//           iVar4 = iVar9;
//         }
//       }
//       GsSortLine(&local_48,OTablePt,(ushort)iVar4);
//     }
//     local_50 = local_58;
//     local_4c = local_54;
//   }
//   iVar9 = end->vy;
//   iVar8 = end->vx - start->vx;
//   iVar10 = end->vz - start->vz;
//   iVar3 = start->vy;
//   local_34 = ratan2(-iVar8,-iVar10);
//   lVar2 = SquareRoot0(iVar8 * iVar8 + iVar10 * iVar10);
//   local_38 = ratan2(iVar9 - iVar3,lVar2);
//   dim = DAT_80097f28;
//   (DAT_80097f28->locate).coord.t[0] = local_30;
//   (dim->locate).coord.t[1] = unaff_s8;
//   (dim->locate).coord.t[2] = unaff_s7;
//   (dim->rotate).vx = (short)local_38;
//   (dim->rotate).vy = (short)local_34;
//   (dim->rotate).vz = 0;
//   UpdateCoordinate(dim);
//   DrawModel(DAT_80097f28);
//   return;
// }
