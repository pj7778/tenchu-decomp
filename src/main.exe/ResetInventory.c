#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ResetInventory", ResetInventory);

// triage: EASY — 33 insns, 2 loop, 0 callees, ~0.07 to DisposeWeapon
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void ResetInventory(void)
//
// {
//   int iVar1;
//   int iVar2;
//   int iVar3;
//
//   PersistentState._7_1_ = 0xff;
//   iVar3 = 1;
//   iVar1 = 0x10000;
//   do {
//     (&PersistentState.field_0x7)[iVar1 >> 0x10] = 0;
//     iVar3 = iVar3 + 1;
//     iVar2 = iVar3 * 0x10000 >> 0x10;
//     iVar1 = iVar3 * 0x10000;
//   } while (iVar2 < 9);
//   while (iVar2 < 0x14) {
//     (&PersistentState.field_0x7)[(short)iVar3] = 0xfe;
//     iVar3 = iVar3 + 1;
//     iVar2 = iVar3 * 0x10000 >> 0x10;
//   }
//   return;
// }
