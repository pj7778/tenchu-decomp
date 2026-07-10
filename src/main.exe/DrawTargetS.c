#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void DrawTargetS(long x, long y, long z, long color);
 *     EFFECT.C:442, 23 src lines, frame 56 bytes, saved-reg mask 0x801f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s0       long x
 *     param $s1       long y
 *     param $s3       long z
 *     param $a3       long color
 *     stack sp+16     struct GsLINE line
 *     reg   $a2       int z
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawTargetS", DrawTargetS);

// triage: EASY — 65 insns, 1 callees, ~0.08 to GetAbsolutePosition
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void DrawTargetS(short param_1,short param_2,int param_3,int param_4)
//
// {
//   short sVar1;
//   int iVar2;
//   short sVar3;
//   GsLINE local_28;
//
//   param_3 = param_3 >> 2;
//   if (param_3 < 0) {
//     iVar2 = 0;
//   }
//   else {
//     iVar2 = 0x4e1;
//     if (param_3 < 0x4e2) {
//       iVar2 = param_3;
//     }
//   }
//   local_28.r = (uchar)((uint)param_4 >> 0x10);
//   local_28.attribute = 0;
//   local_28.g = (uchar)((uint)param_4 >> 8);
//   local_28.b = (uchar)param_4;
//   if (param_4 < 0) {
//     sVar3 = param_1 + -0x14;
//     sVar1 = param_2 + -0x14;
//     param_1 = param_1 + 0x14;
//     param_2 = param_2 + 0x14;
//   }
//   else {
//     sVar3 = param_1 + -2;
//     sVar1 = param_2 + -2;
//     param_1 = param_1 + 2;
//     param_2 = param_2 + 2;
//   }
//   local_28.x0 = sVar3;
//   local_28.y0 = sVar1;
//   local_28.x1 = param_1;
//   local_28.y1 = param_2;
//   GsSortLine(&local_28,OTablePt,(ushort)iVar2);
//   local_28.x0 = param_1;
//   local_28.y0 = sVar1;
//   local_28.x1 = sVar3;
//   local_28.y1 = param_2;
//   GsSortLine(&local_28,OTablePt,(ushort)iVar2);
//   return;
// }
