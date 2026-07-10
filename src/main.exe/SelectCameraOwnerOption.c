#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/SelectCameraOwnerOption", SelectCameraOwnerOption);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/SelectCameraOwnerOption", debug_menu_enemy_layout_select_camera_owner__override__prt_8005ba50_8d2134c3);

// triage: EASY — 68 insns, 1 loop, frame 0x2a8, 2 callees, ~0.12 to AdtFntOpen
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Stack objects: 0x2a8 frame — buffer casts / spills

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SelectCameraOwnerOption(void)
//
// {
//   Humanoid *pHVar1;
//   char *buffer;
//   int iVar2;
//   int iVar3;
//   Humanoid **ppHVar4;
//   TAdtSelect local_298 [36];
//   char acStack_178 [352];
//
//   if (Humans < 0x23) {
//     iVar2 = 0;
//     if (0 < Humans) {
//       ppHVar4 = HumanGroup;
//       buffer = acStack_178;
//       iVar3 = iVar2;
//       do {
//         sprintf(buffer,s__d_80097d70,iVar3);
//         local_298[iVar3].name = buffer;
//         pHVar1 = *ppHVar4;
//         ppHVar4 = ppHVar4 + 1;
//         buffer = buffer + 10;
//         iVar2 = iVar3 + 1;
//         local_298[iVar3].value = (ulong)pHVar1;
//         iVar3 = iVar2;
//       } while (iVar2 < Humans);
//     }
//     local_298[iVar2].name = (char *)0x0;
//     CamState.Owner = (Humanoid *)AdtSelect("select camera owner",local_298,0);
//     ViewInfo.vpx = ((CamState.Owner)->model->locate).coord.t[0];
//     ViewInfo.vry = ((CamState.Owner)->model->locate).coord.t[1];
//     ViewInfo.vpz = ((CamState.Owner)->model->locate).coord.t[2];
//     ViewInfo.vpy = ViewInfo.vry + -5000;
//     ViewInfo.vrx = ViewInfo.vpx;
//     ViewInfo.vrz = ViewInfo.vpz;
//   }
//   return;
// }
