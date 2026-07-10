#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AdtVsprintf", AdtVsprintf);

// triage: EASY — 62 insns, 1 loop, 2 callees, ~0.17 to AddXG4
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int AdtVsprintf(undefined4 *param_1,char *param_2,uint param_3,char *param_4)
//
// {
//   int iVar1;
//   int iVar2;
//   undefined4 *puVar3;
//   undefined4 unaff_s0;
//   undefined4 local_40 [4];
//   undefined4 local_30;
//   undefined4 local_2c;
//   undefined4 local_28;
//   undefined4 local_24;
//   undefined4 local_20;
//   undefined4 local_1c;
//
//   iVar1 = strlen(param_4);
//   iVar2 = 0;
//   if ((uint)(iVar1 << 1) <= param_3) {
//     iVar1 = 0;
//     puVar3 = local_40;
//     do {
//       iVar1 = iVar1 + 1;
//       *puVar3 = *param_1;
//       puVar3 = puVar3 + 1;
//       param_1 = param_1 + 1;
//     } while (iVar1 < 10);
//     iVar2 = sprintf(param_2,param_4,local_40[0],local_40[1],local_40[2],local_40[3],local_30,
//                     local_2c,local_28,local_24,local_20,local_1c,unaff_s0);
//   }
//   return iVar2;
// }
