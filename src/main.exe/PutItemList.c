#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/PutItemList", PutItemList);

// triage: MEDIUM — 126 insns, mul/div, 1 loop, 1 callees, ~0.06 to SetupMotionRegist
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void PutItemList(void)
//
// {
//   uint uVar1;
//   uchar uVar2;
//   int iVar3;
//   GsOT *ot;
//   uint uVar4;
//   int iVar5;
//   short sVar6;
//   int iVar7;
//
//   sVar6 = 0x8c;
//   DAT_80097b1e = 0xffff;
//   iVar7 = 0;
//   for (iVar5 = 0; uVar2 = NumberImage.u, iVar5 < 0x19; iVar5 = iVar5 + 1) {
//     uVar4 = (uint)(CamState.Owner)->item[iVar5];
//     if (uVar4 != 0) {
//       if (uVar4 != 0xff) {
//         NumberImage.w = 4;
//         NumberImage.x = sVar6 + 0x16;
//         NumberImage.y = 100;
//         do {
//           uVar1 = (int)uVar4 / 10;
//           NumberImage.u = uVar2 + ((char)uVar4 + (char)uVar1 * -10) * '\x04';
//           GsSortSprite(&NumberImage,OTablePt,0);
//           NumberImage.x = NumberImage.x + -6;
//           uVar4 = uVar1;
//         } while (uVar1 != 0);
//       }
//       ot = OTablePt;
//       NumberImage.u = uVar2;
//       if (DAT_80097b20 == iVar5) {
//         CursorImage.y = 0x5c;
//         CursorImage.scalex = 0x1000;
//         CursorImage.scaley = 0x1000;
//         CursorImage.rotate = CursorImage.rotate + -0x6000;
//         CursorImage.x = sVar6;
//         GsSortSprite(&CursorImage,OTablePt,1);
//         ot = OTablePt;
//         iVar3 = *(int *)((int)&DAT_8008e5bc + iVar7);
//         DAT_80097b1e = (undefined2)iVar5;
//         *(short *)(iVar3 + 0x6c) = sVar6;
//         *(undefined2 *)(iVar3 + 0x6e) = 0x5c;
//         *(undefined2 *)(iVar3 + 0x84) = 0x1000;
//         *(undefined2 *)(iVar3 + 0x86) = 0x1000;
//       }
//       else {
//         iVar3 = *(int *)((int)&DAT_8008e5bc + iVar7);
//         *(short *)(iVar3 + 0x6c) = sVar6;
//         *(undefined2 *)(iVar3 + 0x6e) = 0x5c;
//         *(undefined2 *)(iVar3 + 0x84) = 0xaaa;
//         *(undefined2 *)(iVar3 + 0x86) = 0xaaa;
//       }
//       sVar6 = sVar6 + -0x14;
//       GsSortSprite((GsSPRITE *)(iVar3 + 0x68),ot,0);
//     }
//     iVar7 = iVar7 + 4;
//   }
//   return;
// }
