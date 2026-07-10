#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_800566fc", FUN_800566fc);

// triage: EASY — 40 insns, 1 loop, 3 callees, ~0.06 to FUN_800565f0
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_800566fc(void)
//
// {
//   short sVar1;
//   int iVar2;
//
//   iVar2 = 0;
//   do {
//     sVar1 = (short)iVar2;
//     iVar2 = iVar2 + 1;
//     (&PersistentState.field_0x40c)[(int)sVar1 + (uint)(byte)PersistentState._4_1_ * 0x20] =
//          (&PersistentState.field_0x27)[sVar1];
//   } while (iVar2 * 0x10000 >> 0x10 < 0x14);
//   FadeOutDirect(0x20,2,'\b','\b');
//   FUN_80038ce0();
//   PersistentState._6_1_ = 0xff;
//   PersistentState._72_1_ = PersistentState._72_1_ & 0xfe;
//   FUN_8004f6c0(0x10);
//   return;
// }
