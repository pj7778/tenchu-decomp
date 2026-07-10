#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_output_edit_camera_settings", debug_output_edit_camera_settings);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_output_edit_camera_settings", debug_output_edit_camera_settings__override__prt_800309b0_aac269c2);

// triage: MEDIUM — 157 insns, 1 loop, 1 callees, ~0.10 to initialise_default_player_cameras_
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8003076c(ushort param_1)
//
// {
//   undefined4 uVar1;
//   undefined4 uVar2;
//   undefined *puVar3;
//   short *psVar4;
//   undefined4 *puVar5;
//   char *pcVar6;
//   char *pcVar7;
//   int iVar8;
//   undefined **ppuVar9;
//
//   DAT_80097f20 = param_1 & (param_1 ^ DAT_80097f22);
//   if (((DAT_80097f20 & 4) != 0) && (DAT_80097a24 = DAT_80097a24 + 1, 3 < DAT_80097a24)) {
//     DAT_80097a24 = 0;
//   }
//   puVar5 = &DAT_800be8f8;
//   psVar4 = (short *)(&DAT_800be8f8)[DAT_80097a24];
//   DAT_80097f22 = param_1;
//   if ((param_1 & 0x1000) != 0) {
//     psVar4[2] = psVar4[2] + -0x32;
//   }
//   if ((DAT_80097f22 & 0x4000) != 0) {
//     psVar4[2] = psVar4[2] + 0x32;
//   }
//   if ((DAT_80097f22 & 0x10) != 0) {
//     psVar4[1] = psVar4[1] + -0x32;
//   }
//   if ((DAT_80097f22 & 0x40) != 0) {
//     psVar4[1] = psVar4[1] + 0x32;
//   }
//   if ((DAT_80097f22 & 0x80) != 0) {
//     *psVar4 = *psVar4 + -0x32;
//   }
//   if ((DAT_80097f22 & 0x20) != 0) {
//     *psVar4 = *psVar4 + 0x32;
//   }
//   puVar3 = PTR_DAT_80097a10;
//   uVar2 = DAT_80011bc8;
//   uVar1 = DAT_80011bc4;
//   iVar8 = 0;
//   if ((DAT_80097f22 & 3) == 3) {
//     *(undefined4 *)PTR_DAT_80097a10 = DAT_80011bc0;
//     *(undefined4 *)(puVar3 + 4) = uVar1;
//     *(undefined4 *)(puVar3 + 8) = uVar2;
//     uVar2 = DAT_80011bd4;
//     uVar1 = DAT_80011bd0;
//     *(undefined4 *)(puVar3 + 0xc) = DAT_80011bcc;
//     *(undefined4 *)(puVar3 + 0x10) = uVar1;
//     *(undefined4 *)(puVar3 + 0x14) = uVar2;
//     uVar1 = DAT_80011bdc;
//     *(undefined4 *)(puVar3 + 0x18) = DAT_80011bd8;
//     *(undefined4 *)(puVar3 + 0x1c) = uVar1;
//   }
//   ppuVar9 = &PTR_DAT_80089fd0;
//   do {
//     pcVar6 = (char *)0x20;
//     if (DAT_80097a24 == iVar8) {
//       pcVar6 = (char *)0x2a;
//     }
//     psVar4 = (short *)*puVar5;
//     iVar8 = iVar8 + 1;
//     puVar5 = puVar5 + 1;
//     pcVar7 = *ppuVar9;
//     ppuVar9 = ppuVar9 + 1;
//     FntPrint("%c%s %6d %6d %6d\n",pcVar6,pcVar7,(int)*psVar4);
//   } while (iVar8 < 4);
//   return;
// }
