#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8005f380", FUN_8005f380);

// triage: MEDIUM — 118 insns, 5 loop, frame 0x858, 6 callees, ~0.10 to cd_control
// likely-relevant cookbook sections:
//   - Loops: 5 back-edge(s) — for/while/do vs goto shape
//   - Stack objects: 0x858 frame — buffer casts / spills

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8005f380(undefined1 *buffer,int param_2,int param_3,int length)
//
// {
//   int iVar1;
//   int iVar2;
//   int iVar3;
//   undefined1 *puVar4;
//   int iVar5;
//   int iVar6;
//   int iVar7;
//   undefined1 *puVar8;
//   CdlLOC aCStack_848 [3];
//   undefined1 auStack_83c [2052];
//   u_char local_38 [8];
//   CdlLOC aCStack_30 [2];
//
//   if (length < 1) {
//     return;
//   }
//   do {
//     local_38[0] = 0xa0;
//     CdIntToPos(param_2,aCStack_30);
//     while( true ) {
//       iVar2 = CdControlB('\x0e',local_38,(u_char *)0x0);
//       iVar5 = 3;
//       if (iVar2 != 0) break;
//       VSync(0);
//     }
//     do {
//       VSync(iVar5);
//       iVar3 = CdControlB('\x06',&aCStack_30[0].minute,(u_char *)0x0);
//       iVar5 = 0;
//       puVar8 = buffer;
//       iVar2 = param_2;
//       iVar7 = length;
//       iVar6 = param_3;
//     } while (iVar3 == 0);
//     while( true ) {
//       if (iVar7 < 1) {
//         while (iVar2 = CdControlB('\t',&aCStack_30[0].minute,(u_char *)0x0), iVar2 == 0) {
//           VSync(0);
//         }
//         return;
//       }
//       iVar5 = CdReady(0,(u_char *)0x0);
//       if ((iVar5 != 1) || (iVar5 = CdGetSector(aCStack_848,0x203), iVar5 == 0)) break;
//       iVar5 = CdPosToInt(aCStack_848);
//       if (iVar5 == iVar2) {
//         iVar5 = (iVar6 + iVar7) - iVar6;
//         if (0x800 < iVar6 + iVar7) {
//           iVar5 = 0x800 - iVar6;
//         }
//         iVar3 = 0;
//         puVar4 = puVar8;
//         if (0 < iVar5) {
//           do {
//             iVar1 = iVar3 + iVar6;
//             iVar3 = iVar3 + 1;
//             *puVar4 = auStack_83c[iVar1];
//             puVar4 = puVar8 + iVar3;
//           } while (iVar3 < iVar5);
//         }
//         puVar8 = puVar8 + iVar5;
//         iVar7 = iVar7 - iVar5;
//         iVar6 = 0;
//         iVar2 = iVar2 + 1;
//       }
//       else {
//         CdIntToPos(iVar2,aCStack_30);
//         while (iVar5 = CdControlB('\x06',&aCStack_30[0].minute,(u_char *)0x0), iVar5 == 0) {
//           VSync(0);
//         }
//       }
//     }
//   } while( true );
// }
