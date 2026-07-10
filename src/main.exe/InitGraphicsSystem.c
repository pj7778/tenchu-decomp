#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/InitGraphicsSystem", InitGraphicsSystem);

// triage: MEDIUM — 103 insns, 17 callees, ~0.07 to CdaStop

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// void InitGraphicsSystem(void)
//
// {
//   int iVar1;
//
//   SetDispMask(0);
//   GsInitGraph(0x140,0xf0,0x34,1,0);
//   GsDefDispBuff(0,0,0,0xf0);
//   GsInit3D();
//   GsInitCoordinate2((GsCOORDINATE2 *)0x0,&World.locate);
//   UpdateCoordinate(&World);
//   _DrawTMDmode = 0;
//   SetDepthQ(0xffff810c,0x2f282e0);
//   _DepthPoint = 0x4e2;
//   _SlightPoint = 0x96;
//   Fog.dqa = -0x7ef4;
//   Fog.dqb = 0x2f282e0;
//   Fog.bfc = '\0';
//   Fog.gfc = '\0';
//   Fog.rfc = '\0';
//   GsSetFogParam(&Fog);
//   GsSetLightMode(0);
//   GsSetAmbient(0x800,0x800,0x800);
//   GsSetProjection(300);
//   ViewInfo.vpx = 0;
//   ViewInfo.vpy = 0;
//   ViewInfo.vpz = -1000;
//   ViewInfo.vrx = 0;
//   ViewInfo.vry = 0;
//   ViewInfo.vrz = 0;
//   ViewInfo.rz = 0;
//   ViewInfo.super = &World.locate;
//   GsSetRefView2(&ViewInfo);
//   GsSetNearClip(0);
//   AdtFntLoad(0x3c0,0x100);
//   AdtFntOpen(0xffffff60,0xffffff98,0x140,0xd0,0,0x400);
//   OTable[1].length = 0xb;
//   OTable[0].length = 0xb;
//   OTable[0].org = ZSortTable[0];
//   OTable[1].org = ZSortTable[0x400];
//   iVar1 = VSync(-1);
//   DAT_80010e70 = DAT_80010e70 + iVar1;
//   srand(DAT_80010e70);
//   return;
// }
