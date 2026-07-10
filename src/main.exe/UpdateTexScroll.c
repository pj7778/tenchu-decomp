#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void UpdateTexScroll(struct TexScroll *tscr);
 *     EFFECT.C:1884, 24 src lines, frame 32 bytes, saved-reg mask 0x80030000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s1       struct TexScroll * tscr
 *     reg   $s0       struct DR_MOVE * prim
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/UpdateTexScroll", UpdateTexScroll);

// triage: EASY — 68 insns, mul/div, 4 callees, ~0.08 to AddXG4
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void UpdateTexScroll(int param_1)
//
// {
//   uint uVar1;
//   DR_MOVE *p;
//   int iVar2;
//   int iVar3;
//
//   uVar1 = (int)*(short *)(param_1 + 0x18) << 4;
//   if (uVar1 == 0) {
//     trap(0x1c00);
//   }
//   *(short *)(param_1 + 4) =
//        (short)((uint)((int)*(short *)(param_1 + 4) + (int)*(short *)(param_1 + 8)) % uVar1);
//   uVar1 = (int)*(short *)(param_1 + 0x1a) << 4;
//   if (uVar1 == 0) {
//     trap(0x1c00);
//   }
//   *(short *)(param_1 + 6) =
//        (short)((uint)((int)*(short *)(param_1 + 6) + (int)*(short *)(param_1 + 10)) % uVar1);
//   iVar2 = (int)*(short *)(param_1 + 4);
//   if (iVar2 < 0) {
//     iVar2 = iVar2 + 0xf;
//   }
//   iVar3 = (int)*(short *)(param_1 + 6);
//   *(short *)(param_1 + 0x14) = *(short *)(param_1 + 0x10) + (short)(iVar2 >> 4);
//   if (iVar3 < 0) {
//     iVar3 = iVar3 + 0xf;
//   }
//   *(short *)(param_1 + 0x16) = *(short *)(param_1 + 0x12) + (short)(iVar3 >> 4);
//   p = (DR_MOVE *)GsGetWorkBase();
//   GsSetWorkBase(p + 1);
//   SetDrawMove(p,(RECT *)(param_1 + 0x14),(int)*(short *)(param_1 + 0xc),
//               (int)*(short *)(param_1 + 0xe));
//   AddPrim(OTablePt->org,p);
//   return;
// }
