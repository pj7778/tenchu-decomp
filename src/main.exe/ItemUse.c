#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ItemUse", ItemUse);

// triage: EASY — 67 insns, mul/div, 2 callees, ~0.07 to AttackAnimal
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// short ItemUse(void)
//
// {
//   short sVar1;
//   int iVar2;
//
//   if (Me_THINK_C->motion->count != 0) {
//     return 5;
//   }
//   if (Me_THINK_C->status != 5) {
//     return 5;
//   }
//   if ((Me_THINK_C->item[3] != '\0') &&
//      (iVar2 = ((int)Me_THINK_C->lifemax / 3) * 0x10000 >> 0x10, Me_THINK_C->life < iVar2)) {
//     ReqItemDefault(Me_THINK_C,ITEM_KUSURI);
//     return (short)iVar2;
//   }
//   if (Me_THINK_C->item[1] != '\0') {
//     iVar2 = (int)Degree;
//     if (iVar2 < 0) {
//       iVar2 = -iVar2;
//     }
//     sVar1 = 0xe00;
//     if (iVar2 < 100) goto LAB_8002dd00;
//   }
//   if (Me_THINK_C->item[4] == '\0') {
//     return 0;
//   }
//   iVar2 = (int)Degree;
//   if (iVar2 < 0) {
//     iVar2 = -iVar2;
//   }
//   sVar1 = 0xf02;
//   if (iVar2 >= 300) {
//     return (ushort)(iVar2 < 300);
//   }
// LAB_8002dd00:
//   sVar1 = SetNowMotion(Me_THINK_C,sVar1,1);
//   return sVar1;
// }
