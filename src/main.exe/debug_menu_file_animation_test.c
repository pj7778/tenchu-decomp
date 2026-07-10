#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_menu_file_animation_test", debug_menu_file_animation_test);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_menu_file_animation_test", debug_menu_file_animation_test__override__prt_800501f8_8d2134c3);

// triage: MEDIUM — 65 insns, 1 loop, frame 0x328, 4 callees, ~0.06 to SetupAfterimage
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py
//   - Stack objects: 0x328 frame — buffer casts / spills

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_800501b0(void)
//
// {
//   short sVar1;
//   int iVar2;
//   ulong uVar3;
//   short *psVar4;
//   char *buffer;
//   short *psVar5;
//   int iVar6;
//   char acStack_318 [256];
//   TAdtSelect local_218 [64];
//
//   buffer = acStack_318;
//   sVar1 = *DAT_80097cb8;
//   iVar6 = 0;
//   if (*DAT_80097cb8 != -1) {
//     psVar4 = DAT_80097cb8 + 1;
//     psVar5 = DAT_80097cb8;
//     iVar2 = iVar6;
//     do {
//       iVar6 = iVar2;
//       if (sVar1 == 0) {
//         sprintf(buffer,&DAT_80097cd0,(int)*psVar4);
//         iVar6 = iVar2 + 1;
//         local_218[iVar2].name = buffer;
//         local_218[iVar2].value = (int)*psVar4;
//         iVar2 = strlen(buffer);
//         buffer = buffer + iVar2 + 1;
//       }
//       psVar5 = psVar5 + 6;
//       sVar1 = *psVar5;
//       psVar4 = psVar4 + 6;
//       iVar2 = iVar6;
//     } while (*psVar5 != -1);
//   }
//   local_218[iVar6].name = s_cancel_80097cd4;
//   local_218[iVar6].value = 0xffffffff;
//   local_218[iVar6 + 1].name = (char *)0x0;
//   uVar3 = AdtSelect("event test",local_218,0);
//   if (uVar3 != 0xffffffff) {
//     CVAsequence((short)uVar3);
//   }
//   return;
// }
