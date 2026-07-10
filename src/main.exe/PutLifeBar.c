#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/PutLifeBar", PutLifeBar);

// triage: MEDIUM — 135 insns, mul/div, 1 loop, 1 callees, ~0.07 to ReqItemNingyo
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void PutLifeBar(int x,int y,int n,int mx,int style)
//
// {
//   undefined2 uVar1;
//   short sVar2;
//   short sVar3;
//   int iVar4;
//   GsOT *pGVar5;
//   uchar uVar6;
//   undefined1 uVar7;
//   int iVar8;
//
//   uVar6 = NumberImage.u;
//   NumberImage.w = 4;
//   sVar2 = (short)x;
//   NumberImage.x = sVar2 + *(short *)(&DAT_8008e418 + style * 0x50);
//   sVar3 = (short)y;
//   NumberImage.y = sVar3 + *(short *)(&DAT_8008e41a + style * 0x50);
//   iVar8 = n;
//   do {
//     iVar4 = iVar8 / 10;
//     NumberImage.u = uVar6 + ((char)iVar8 + (char)iVar4 * -10) * '\x04';
//     GsSortSprite(&NumberImage,OTablePt,0);
//     pGVar5 = OTablePt;
//     NumberImage.x = NumberImage.x + -6;
//     iVar8 = iVar4;
//   } while (iVar4 != 0);
//   iVar8 = style * 0x50;
//   NumberImage.u = uVar6;
//   *(short *)(&DAT_8008e420 + iVar8) = sVar2;
//   *(short *)(iVar8 + -0x7ff71bde) = sVar3;
//   GsSortSprite((GsSPRITE *)(&DAT_8008e41c + style * 0x14),pGVar5,1);
//   if (mx == 0) {
//     trap(0x1c00);
//   }
//   if ((mx == -1) && (*(short *)(&DAT_8008e416 + iVar8) * n == -0x80000000)) {
//     trap(0x1800);
//   }
//   uVar1 = *(undefined2 *)(&DAT_8008e44a + iVar8);
//   *(short *)(&DAT_8008e44a + iVar8) =
//        *(short *)(&DAT_8008e414 + iVar8) + (short)((*(short *)(&DAT_8008e416 + iVar8) * n) / mx);
//   if (mx < 0) {
//     mx = mx + 3;
//   }
//   uVar7 = 0x80;
//   if ((n <= mx >> 2) && (uVar7 = 0xe6, (GameClock & 1U) == 0)) {
//     uVar7 = 0x80;
//   }
//   (&DAT_8008e456)[iVar8] = uVar7;
//   (&DAT_8008e455)[iVar8] = uVar7;
//   (&DAT_8008e454)[iVar8] = uVar7;
//   pGVar5 = OTablePt;
//   *(short *)(&DAT_8008e444 + iVar8) = sVar2;
//   *(short *)(iVar8 + -0x7ff71bba) = sVar3;
//   GsSortSprite((GsSPRITE *)(&DAT_8008e440 + style * 0x14),pGVar5,0);
//   *(undefined2 *)(&DAT_8008e44a + iVar8) = uVar1;
//   return;
// }
