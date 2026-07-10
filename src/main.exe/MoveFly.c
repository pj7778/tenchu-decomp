#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/MoveFly", MoveFly);

// triage: MEDIUM — 122 insns, mul/div, 1 callees, ~0.07 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void MoveFly(int *item,int *param)
//
// {
//   int iVar1;
//   uint uVar2;
//   int iVar3;
//   undefined4 uVar4;
//   int iVar5;
//   undefined4 uVar6;
//   undefined4 uVar7;
//   int iVar8;
//   int iVar9;
//   uint uVar10;
//
//   if ((char)param[10] == '\0') {
//     uVar10 = (uint)*(byte *)(param + 9);
//     uVar2 = (uint)*(byte *)((int)param + 0x25);
//     if (uVar2 == 0) {
//       trap(0x1c00);
//     }
//     if ((uVar2 == 0xffffffff) && (uVar10 == 0x80000)) {
//       trap(0x1800);
//     }
//     iVar1 = 0x1000 - (uVar10 << 0xc) / uVar2;
//     iVar3 = iVar1 * iVar1;
//     if (iVar3 < 0) {
//       iVar3 = iVar3 + 0xfff;
//     }
//     iVar3 = iVar3 >> 0xc;
//     iVar9 = iVar1 * -2 + 0x1000 + iVar3;
//     iVar8 = iVar1 * 2 + iVar3 * -2;
//     iVar1 = iVar9 * *param + iVar8 * param[6] + iVar3 * param[3];
//     if (iVar1 < 0) {
//       iVar1 = iVar1 + 0xfff;
//     }
//     iVar5 = iVar9 * param[1] + iVar8 * param[7] + iVar3 * param[4];
//     if (iVar5 < 0) {
//       iVar5 = iVar5 + 0xfff;
//     }
//     iVar3 = iVar9 * param[2] + iVar8 * param[8] + iVar3 * param[5];
//     if (iVar3 < 0) {
//       iVar3 = iVar3 + 0xfff;
//     }
//     if (uVar10 == 0) {
//       iVar8 = item[4];
//       uVar4 = *(undefined4 *)(iVar8 + 0x18);
//       uVar6 = *(undefined4 *)(iVar8 + 0x1c);
//       uVar7 = *(undefined4 *)(iVar8 + 0x20);
//       *param = 0;
//       *(undefined1 *)((int)param + 10) = 0;
//       *(undefined1 *)(param + 10) = 1;
//       *(short *)(param + 1) = (short)(iVar1 >> 0xc) - (short)uVar4;
//       *(short *)((int)param + 6) = (short)(iVar5 >> 0xc) - (short)uVar6;
//       *(short *)(param + 2) = (short)(iVar3 >> 0xc) - (short)uVar7;
//     }
//     else {
//       *(byte *)(param + 9) = *(byte *)(param + 9) - 1;
//     }
//     *(int *)(item[4] + 0x18) = iVar1 >> 0xc;
//     *(int *)(item[4] + 0x1c) = iVar5 >> 0xc;
//     *(int *)(item[4] + 0x20) = iVar3 >> 0xc;
//   }
//   else if ((char)param[10] == '\x01') {
//     MoveKorogari((tag_TItem *)item,(param_korogari *)param);
//   }
//   return;
// }
