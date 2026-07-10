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

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8003a148", FUN_8003a148);

// triage: MEDIUM — 88 insns, mul/div, 5 callees, ~0.06 to ReqItemShinsoku
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8003a148(GsSPRITE *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
//                  int param_5,GsCOORDINATE2 *param_6,short param_7)
//
// {
//   short sVar1;
//   long lVar2;
//   int iVar3;
//   int iVar4;
//   undefined4 local_18;
//   short local_14;
//
//   if (param_6 == (GsCOORDINATE2 *)0x0) {
//     FUN_800396c0(param_2,param_3,param_4,&local_18);
//   }
//   else {
//     Scratchpad._32_2_ = (undefined2)param_2;
//     Scratchpad._34_2_ = (undefined2)param_3;
//     Scratchpad._36_2_ = (undefined2)param_4;
//     GsGetLs(param_6,(MATRIX *)Scratchpad);
//     GsSetLsMatrix((MATRIX *)Scratchpad);
//     lVar2 = RotTransPers((SVECTOR *)(Scratchpad + 0x20),&local_18,(long *)(Scratchpad + 0x28),
//                          (long *)(Scratchpad + 0x2c));
//     local_14 = (short)lVar2;
//   }
//   iVar3 = (int)local_14;
//   if (0x24 < iVar3) {
//     if (iVar3 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar3 == -1) && (param_5 * 300 == -0x80000000)) {
//       trap(0x1800);
//     }
//     sVar1 = (short)((param_5 * 300) / iVar3) + 1;
//     param_1->scaley = sVar1;
//     param_1->scalex = sVar1;
//     param_1->x = (short)local_18;
//     param_1->y = local_18._2_2_;
//     iVar3 = (int)local_14 + (int)param_7 >> 2;
//     if (iVar3 < 0) {
//       iVar4 = 0;
//     }
//     else {
//       iVar4 = 0x4e1;
//       if (iVar3 < 0x4e2) {
//         iVar4 = iVar3;
//       }
//     }
//     GsSortSprite(param_1,OTablePt,(ushort)iVar4);
//   }
//   return;
// }
