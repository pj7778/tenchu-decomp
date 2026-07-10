#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/subAfsFindFile", subAfsFindFile);

// triage: EASY — 45 insns, 1 loop, 1 callees, ~0.13 to AdtFntOpen
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// uint subAfsFindFile(int param_1,char *param_2,ushort param_3)
//
// {
//   int iVar1;
//   int iVar2;
//   uint uVar3;
//
//   uVar3 = 0;
//   if (*(int *)(param_1 + 0x10) != 0) {
//     iVar2 = 0;
//     do {
//       iVar1 = strncmp(param_2,(char *)(*(int *)(param_1 + 0xc) + iVar2 + 0x10),0x14);
//       if ((iVar1 == 0) && ((param_3 & *(ushort *)(iVar2 + *(int *)(param_1 + 0xc))) != 0)) {
//         return uVar3;
//       }
//       uVar3 = uVar3 + 1;
//       iVar2 = iVar2 + 0x24;
//     } while (uVar3 < *(uint *)(param_1 + 0x10));
//   }
//   return 0xffffffff;
// }
