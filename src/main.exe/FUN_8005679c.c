#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern struct GsOT *OTablePt;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8005679c", FUN_8005679c);

// triage: MEDIUM — 71 insns, mul/div, 1 loop, 1 callees, ~0.06 to BriefingAndInventorySelectionScreen
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8005679c(GsSPRITE *param_1,uint param_2,short param_3,short param_4)
//
// {
//   uchar uVar1;
//   bool bVar2;
//   short sVar3;
//
//   param_1->x = param_3;
//   param_1->y = param_4;
//   if ((short)param_2 < 0) {
//     param_2 = -(int)(short)param_2;
//     bVar2 = true;
//   }
//   else {
//     bVar2 = false;
//   }
//   do {
//     sVar3 = (short)param_2;
//     param_2 = (int)sVar3 / 10;
//     uVar1 = param_1->u;
//     param_1->u = uVar1 + (char)((uint)(((int)sVar3 % 10) * 0x10000) >> 0x10) * (char)param_1->w;
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//     param_1->x = param_1->x + -0xc;
//   } while ((param_2 & 0xffff) != 0);
//   if (bVar2) {
//     param_1->u = uVar1 + (char)param_1->w * '\n';
//     GsSortSprite(param_1,OTablePt,0);
//     param_1->u = uVar1;
//   }
//   return;
// }
