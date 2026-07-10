#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/EndDrawing", EndDrawing);

// triage: MEDIUM — 134 insns, mul/div, 7 callees, ~0.04 to ProcItemDrop
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void EndDrawing(short sync)
//
// {
//   PACKET *pPVar1;
//   int iVar2;
//   uint uVar3;
//   int iVar4;
//
//   if ((GameClock == (GameClock / 0x1e) * 0x1e) && (SkipFrame == 0)) {
//     pPVar1 = GsGetWorkBase();
//     DAT_800976b8 = pPVar1 + DrawingPage * -0x10000 + 0x7ff67fc0;
//     if ((PACKET *)0x10000 < DAT_800976b8) {
//       DAT_800976b8 = (PACKET *)0x10000;
//     }
//   }
//   if (SkipFrame == 1) {
//     uVar3 = (uint)(ushort)DrawingPage;
//     sync = (short)(((uint)(ushort)sync << 0x10) >> 0xf);
//     SkipFrame = 0;
//     DrawingPage = (short)(1 - uVar3);
//     OTablePt = OTable + ((int)((1 - uVar3) * 0x10000) >> 0x10);
//   }
//   else if (SkipFrame < 2) {
//     if ((SkipFrame == 0) && (iVar4 = VSync(1), sync * -0xf0 + -10 < iVar4)) {
//       SkipFrame = 1;
//       return;
//     }
//   }
//   else if (SkipFrame == 2) {
//     SkipFrame = 0;
//   }
//   OTablePt->org[0x7fe] = OTablePt->org[0x4e2];
//   iVar4 = (int)sync;
//   if (iVar4 < 1) {
//     DrawSync(0);
//     VSync(-iVar4);
//   }
//   else {
//     iVar2 = VSync(-1);
//     if (iVar2 - time < iVar4) {
//       VSync(iVar4);
//     }
//     time = VSync(-1);
//     ResetGraph(1);
//   }
//   GsSwapDispBuff();
//   GsSortClear(Fog.rfc,Fog.gfc,Fog.bfc,OTablePt);
//   GsDrawOt(OTablePt);
//   return;
// }
