#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/check_for_known_button_combination", check_for_known_button_combination);

// triage: MEDIUM — 72 insns, 4 loop, 0 callees, ~0.01 to ReturnNormal
// likely-relevant cookbook sections:
//   - Loops: 4 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int FUN_80051f64(undefined2 param_1,short param_2)
//
// {
//   int iVar1;
//   undefined *puVar2;
//   short *psVar3;
//   int iVar4;
//   short *psVar5;
//   undefined **ppuVar6;
//
//   iVar4 = 0xb;
//   if (param_2 != 0) {
//     do {
//       iVar1 = iVar4 + -1;
//       (&DAT_800c2cf0)[iVar4] = (&DAT_800c2cf0)[iVar1];
//       iVar4 = iVar1;
//     } while (0 < iVar1);
//     DAT_800c2cf0 = param_1;
//     if (PTR_DAT_8008eddc != (undefined *)0x0) {
//       ppuVar6 = &PTR_DAT_8008eddc;
//       puVar2 = PTR_DAT_8008eddc;
//       do {
//         iVar4 = 0;
//         psVar3 = (short *)(puVar2 + 2);
//         psVar5 = &DAT_800c2cf0;
//         if (*(short *)(puVar2 + 2) == -1) {
// LAB_80052028:
//           iVar4 = 0xb;
//           do {
//             iVar1 = iVar4 + -1;
//             (&DAT_800c2cf0)[iVar4] = (&DAT_800c2cf0)[iVar1];
//             iVar4 = iVar1;
//           } while (0 < iVar1);
//           DAT_800c2cf0 = 0;
//           return (int)*(short *)*ppuVar6;
//         }
//         do {
//           iVar1 = iVar4 << 1;
//           if (*psVar3 != *psVar5) goto LAB_80052014;
//           psVar3 = psVar3 + 1;
//           iVar4 = iVar4 + 1;
//           psVar5 = psVar5 + 1;
//         } while (*psVar3 != -1);
//         iVar1 = iVar4 * 2;
// LAB_80052014:
//         if (*(short *)(iVar1 + (int)(puVar2 + 2)) == -1) goto LAB_80052028;
//         ppuVar6 = ppuVar6 + 1;
//         puVar2 = *ppuVar6;
//       } while (puVar2 != (undefined *)0x0);
//     }
//   }
//   return 0;
// }
