#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AttackBowControl", AttackBowControl);

// triage: EASY — 68 insns, 5 callees, ~0.06 to ProcItemGoshikimai
// likely-relevant cookbook sections:
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// void AttackBowControl(void)
//
// {
//   short sVar1;
//   int iVar2;
//   VECTOR *pVVar3;
//   int in_a0;
//
//   sVar1 = dtM->count;
//   if (sVar1 == 1) {
//     Sound(Me_MOTION_C,2);
//   }
//   else {
//     iVar2 = (in_a0 << 0x10) >> 0xe;
//     if ((*(short *)(&DAT_80097714 + iVar2) <= sVar1) && (sVar1 < *(short *)(&DAT_80097716 + iVar2)))
//     {
//       UpdateOrnament(Me_MOTION_C->weapon[2],0);
//       DrawOrnament(Me_MOTION_C->weapon[2]);
//     }
//   }
//   if (dtM->count == *(short *)(&DAT_80097716 + ((in_a0 << 0x10) >> 0xe))) {
//     pVVar3 = GetAbsolutePosition(Me_MOTION_C->model->object[0xd],0,0,0);
//     FUN_80027554(0x15,pVVar3);
//     Sound(Me_MOTION_C,3);
//   }
//   return;
// }
