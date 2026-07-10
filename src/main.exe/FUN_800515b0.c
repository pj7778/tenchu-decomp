#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_800515b0", FUN_800515b0);

// triage: HARD — 259 insns, mul/div, 3 loop, 1 callees, ~0.07 to BriefingAndInventorySelectionScreen
// likely-relevant cookbook sections:
//   - Loops: 3 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_800515b0(GsSPRITE *param_1,int param_2,short param_3,short param_4,int param_5)
//
// {
//   uchar uVar1;
//   bool bVar2;
//   GsOT *ot;
//   short sVar3;
//   int iVar4;
//   uint uVar5;
//   int iVar6;
//
//   param_1->y = param_4;
//   param_1->x = param_3 + -0x20;
//   uVar5 = (param_2 / 0x1e) / 0x3c;
//   iVar4 = (int)(uVar5 * 0x10000) >> 0x10;
//   bVar2 = false;
//   if (iVar4 < 0) {
//     uVar5 = -iVar4;
//     bVar2 = true;
//   }
//   do {
//     sVar3 = (short)uVar5;
//     uVar5 = (int)sVar3 / 10;
//     uVar1 = param_1->u;
//     param_1->u = uVar1 + (char)((uint)(((int)sVar3 % 10) * 0x10000) >> 0x10) * (char)param_1->w;
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//     param_1->x = param_1->x + -0xc;
//   } while ((uVar5 & 0xffff) != 0);
//   if (bVar2) {
//     param_1->u = uVar1 + (char)param_1->w * '\n';
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//   }
//   iVar6 = (param_2 / 0x1e) % 0x3c;
//   param_1->y = param_4;
//   param_1->x = param_3 + -0xc;
//   uVar5 = iVar6 / 10;
//   iVar4 = (int)(uVar5 * 0x10000) >> 0x10;
//   bVar2 = false;
//   if (iVar4 < 0) {
//     uVar5 = -iVar4;
//     bVar2 = true;
//   }
//   do {
//     sVar3 = (short)uVar5;
//     uVar5 = (int)sVar3 / 10;
//     uVar1 = param_1->u;
//     param_1->u = uVar1 + (char)((uint)(((int)sVar3 % 10) * 0x10000) >> 0x10) * (char)param_1->w;
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//     param_1->x = param_1->x + -0xc;
//   } while ((uVar5 & 0xffff) != 0);
//   if (bVar2) {
//     param_1->u = uVar1 + (char)param_1->w * '\n';
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//   }
//   param_1->x = param_3;
//   uVar5 = iVar6 % 10;
//   iVar4 = (int)(uVar5 * 0x10000) >> 0x10;
//   param_1->y = param_4;
//   if (iVar4 < 0) {
//     uVar5 = -iVar4;
//     bVar2 = true;
//   }
//   else {
//     bVar2 = false;
//   }
//   do {
//     sVar3 = (short)uVar5;
//     uVar5 = (int)sVar3 / 10;
//     uVar1 = param_1->u;
//     param_1->u = uVar1 + (char)((uint)(((int)sVar3 % 10) * 0x10000) >> 0x10) * (char)param_1->w;
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//     param_1->x = param_1->x + -0xc;
//   } while ((uVar5 & 0xffff) != 0);
//   if (bVar2) {
//     param_1->u = uVar1 + (char)param_1->w * '\n';
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//   }
//   if (param_5 != 0) {
//     uVar1 = param_1->u;
//     param_1->u = uVar1 + (char)param_1->w * '\v';
//     ot = OTablePt;
//     param_1->x = param_3 + -0x16;
//     GsSortSprite(param_1,ot,0);
//     param_1->u = uVar1;
//   }
//   return;
// }
