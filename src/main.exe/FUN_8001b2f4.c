#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8001b2f4", FUN_8001b2f4);

// triage: EASY — 28 insns, 1 loop, 0 callees, ~0.03 to FUN_80039c14
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int FUN_8001b2f4(ushort param_1)
//
// {
//   int iVar1;
//   ushort uVar2;
//   int iVar3;
//
//   iVar3 = 0;
//   iVar1 = (int)DAT_800976f6 << 3;
//   uVar2 = param_1;
//   do {
//     if ((param_1 & (byte)(&DAT_80011210)[iVar3]) == 0) {
//       uVar2 = uVar2 & ~(ushort)(byte)(&DAT_80011210)[iVar1];
//     }
//     else {
//       uVar2 = uVar2 | (byte)(&DAT_80011210)[iVar1];
//     }
//     iVar3 = iVar3 + 1;
//     iVar1 = iVar1 + 1;
//   } while (iVar3 < 8);
//   return (int)(short)uVar2;
// }
