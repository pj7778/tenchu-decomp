#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawFrame", DrawFrame);

// triage: MEDIUM — 139 insns, mul/div, 5 callees, ~0.05 to ReqItemShinsoku
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void DrawFrame(tag_EffectSlot *ef)
//
// {
//   uchar uVar1;
//   ushort uVar2;
//   short sVar3;
//   int iVar4;
//   long lVar5;
//   int iVar6;
//   int iVar7;
//   long lVar8;
//   GsCOORDINATE2 *m;
//   long lVar9;
//   short local_18;
//   short local_16;
//   short local_14;
//
//   iVar7 = (int)(ef->param).bleed.vec.vy;
//   iVar4 = iVar7;
//   if (iVar7 < 0) {
//     iVar4 = iVar7 + 3;
//   }
//   iVar4 = (iVar7 + (iVar4 >> 2) * -4) * 0x10000 >> 0x10;
//   uVar1 = (ef->param).frame.mode;
//   if (uVar1 == '\0') {
//     sprFrame[iVar4].b = 0x80;
//     sprFrame[iVar4].g = 0x80;
//     sprFrame[iVar4].r = 0x80;
//     uVar2 = (ef->param).bleed.vec.vy - 1;
//     (ef->param).bleed.vec.vy = uVar2;
//     if ((int)((uint)uVar2 << 0x10) < 1) {
//       uVar1 = (ef->param).frame.mode;
//       (ef->param).bleed.vec.vy = 0x80;
//       (ef->param).frame.mode = uVar1 + '\x01';
//     }
//   }
//   else if (uVar1 == '\x01') {
//     uVar1 = (ef->param).splash.mode;
//     sprFrame[iVar4].b = uVar1;
//     sprFrame[iVar4].g = uVar1;
//     sprFrame[iVar4].r = uVar1;
//     uVar2 = (ef->param).bleed.vec.vy - 0x1d;
//     (ef->param).bleed.vec.vy = uVar2;
//     if ((int)((uint)uVar2 << 0x10) < 1) {
//       ef->proc = (undefined **)0x0;
//     }
//   }
//   lVar5 = (ef->param).blood.px;
//   lVar8 = (ef->param).blood.py;
//   lVar9 = (ef->param).blood.pz;
//   m = (GsCOORDINATE2 *)(ef->param).blood.hint;
//   sVar3 = (ef->param).bleed.vec.vx;
//   if (m == (GsCOORDINATE2 *)0x0) {
//     FUN_800396c0(lVar5,lVar8,lVar9,&local_18);
//   }
//   else {
//     Scratchpad._32_2_ = (undefined2)lVar5;
//     Scratchpad._34_2_ = (undefined2)lVar8;
//     Scratchpad._36_2_ = (undefined2)lVar9;
//     GsGetLs(m,(MATRIX *)Scratchpad);
//     GsSetLsMatrix((MATRIX *)Scratchpad);
//     lVar5 = RotTransPers((SVECTOR *)(Scratchpad + 0x20),(long *)&local_18,
//                          (long *)(Scratchpad + 0x28),(long *)(Scratchpad + 0x2c));
//     local_14 = (short)lVar5;
//   }
//   iVar7 = (int)local_14;
//   if (0x24 < iVar7) {
//     iVar6 = sVar3 * 300;
//     if (iVar7 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar7 == -1) && (iVar6 == -0x80000000)) {
//       trap(0x1800);
//     }
//     sVar3 = (short)(iVar6 / iVar7) + 1;
//     sprFrame[iVar4].scaley = sVar3;
//     sprFrame[iVar4].scalex = sVar3;
//     sprFrame[iVar4].x = local_18;
//     sprFrame[iVar4].y = local_16;
//     iVar7 = local_14 + -0x32 >> 2;
//     if (iVar7 < 0) {
//       iVar6 = 0;
//     }
//     else {
//       iVar6 = 0x4e1;
//       if (iVar7 < 0x4e2) {
//         iVar6 = iVar7;
//       }
//     }
//     GsSortSprite(sprFrame + iVar4,OTablePt,(ushort)iVar6);
//   }
//   return;
// }
