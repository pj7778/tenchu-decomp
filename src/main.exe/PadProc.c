#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/PadProc", PadProc);

// triage: MEDIUM — 92 insns, mul/div, 1 callees, ~0.06 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// void PadProc(void)
//
// {
//   int iVar1;
//
//   ComPad(0,ComBuf[0]);
//   ComPad(0x10,ComBuf[0x11]);
//   iVar1 = PadArrange.attack - PadArrange.time;
//   if (iVar1 < 1) {
//     if (iVar1 + PadArrange.release < 1) {
//       DAT_800be6d8 = 0;
//       DAT_800be6d9 = 0;
//       PadArrange.time = PadArrange.time + 1;
//       return;
//     }
//     iVar1 = PadArrange.pow * (iVar1 + PadArrange.release);
//     if (PadArrange.release == 0) {
//       trap(0x1c00);
//     }
//     if ((PadArrange.release == -1) && (iVar1 == -0x80000000)) {
//       trap(0x1800);
//     }
//     if (PersistentState._93_1_ == '\0') {
//       DAT_800be6d8 = 0;
//       DAT_800be6d9 = 0;
//     }
//     else {
//       DAT_800be6d8 = 0;
//       DAT_800be6d9 = (undefined1)(iVar1 / PadArrange.release);
//     }
//   }
//   else {
//     iVar1 = PadArrange.pow * (PadArrange.attack - iVar1);
//     if (PadArrange.attack == 0) {
//       trap(0x1c00);
//     }
//     if ((PadArrange.attack == -1) && (iVar1 == -0x80000000)) {
//       trap(0x1800);
//     }
//     if (PersistentState._93_1_ == '\0') {
//       DAT_800be6d8 = 0;
//       DAT_800be6d9 = 0;
//     }
//     else {
//       DAT_800be6d8 = 1;
//       DAT_800be6d9 = (undefined1)(iVar1 / PadArrange.attack);
//     }
//   }
//   PadArrange.time = PadArrange.time + 2;
//   return;
// }
