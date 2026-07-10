#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AfsOpenVolume", AfsOpenVolume);

// triage: EASY — 51 insns, 8 callees, ~0.12 to AdtFntLoad
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int AfsOpenVolume(TAFS *handle,char *path)
//
// {
//   uint uVar1;
//   int iVar2;
//   FILE *pFVar3;
//   int iVar4;
//   char acStack_60 [80];
//
//   AfsInit((TAFS *)handle);
//   uVar1 = strlen(path);
//   iVar2 = 1;
//   if (uVar1 < 0x4c) {
//     strcpy(acStack_60,path);
//     strcat(acStack_60,s__VOL_80097e78);
//     pFVar3 = (FILE *)cd_open(acStack_60,0);
//     handle->fpVol = pFVar3;
//     if (pFVar3 == (FILE *)0x0) {
//       AdtMessageBox("AfsOpenVolume: %s open err\n",acStack_60);
//       iVar2 = 1;
//     }
//     else {
//       iVar2 = AfsGetHeader((TAFS *)handle);
//       if (iVar2 == 0) {
//         iVar4 = AfsGetEntry((TAFS *)handle);
//         iVar2 = 0;
//         if (iVar4 != 0) {
//           AdtMessageBox("AfsOpenVolume: Entry error");
//           iVar2 = 3;
//         }
//       }
//       else {
//         AdtMessageBox("AfsOpenVolume: Header error");
//         iVar2 = 2;
//       }
//     }
//   }
//   return iVar2;
// }
