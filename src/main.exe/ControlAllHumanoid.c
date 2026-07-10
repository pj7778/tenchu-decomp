#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ControlAllHumanoid", ControlAllHumanoid);

// triage: EASY — 49 insns, 1 loop, 2 callees, ~0.14 to update_something_for_each_visible_enemy_
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// short ControlAllHumanoid(void)
//
// {
//   short sVar1;
//   Humanoid *human;
//   int iVar2;
//
//   DAT_80097726 = 0;
//   iVar2 = 0;
//   sVar1 = Humans;
//   if (0 < Humans) {
//     do {
//       human = *(Humanoid **)((int)HumanGroup + ((iVar2 << 0x10) >> 0xe));
//       if ((human->attribute & 0x80U) == 0) {
//         if (human->type == 0x85) {
//           FUN_8001aba0();
//           ControlHumanoid(human);
//           FUN_8001aba0();
//         }
//         else {
//           ControlHumanoid(human);
//         }
//       }
//       iVar2 = iVar2 + 1;
//       sVar1 = 0;
//     } while (iVar2 * 0x10000 >> 0x10 < (int)Humans);
//   }
//   return sVar1;
// }
