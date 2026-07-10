#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_menu_player_jump", debug_menu_player_jump);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_menu_player_jump", debug_menu_player_jump__override__prt_8005cc58_394600bf);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_menu_player_jump", debug_menu_player_jump__override__prt_8005cc68_551bd70d);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_menu_player_jump", debug_menu_player_jump__override__prt_8005cc78_551bd70d);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/debug_menu_player_jump", debug_menu_player_jump__override__prt_8005cc88_551bd70d);

// triage: MEDIUM — 173 insns, mul/div, 2 loop, 7 callees, ~0.05 to ReqItemNingyo
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8005cbbc(undefined4 param_1,undefined4 param_2,char *param_3,undefined4 param_4)
//
// {
//   Humanoid *pHVar1;
//   uint uVar2;
//   long lVar3;
//   char *pcVar4;
//   int iVar5;
//   int iVar6;
//   char *local_18;
//   char *local_14;
//   char *local_10;
//
//   pcVar4 = (char *)0x10624dd3;
//   local_18 = (char *)(StagePlayer->locate->vx / 1000);
//   local_14 = (char *)(StagePlayer->locate->vy / 1000);
//   local_10 = (char *)(StagePlayer->locate->vz / 1000);
//   EndDrawing(-2);
//   while( true ) {
//     StartDrawing();
//     FntPrint("player jump\n\n",pcVar4,param_3,param_4);
//     FntPrint("   x= %d\n",local_18,param_3,param_4);
//     FntPrint("   y= %d\n",local_14,param_3,param_4);
//     pcVar4 = local_10;
//     FntPrint("   z= %d\n",local_10,param_3,param_4);
//     FntFlush(-1);
//     EndDrawing(-2);
//     uVar2 = GetRealPad(0);
//     if ((uVar2 & 0x100) != 0) break;
//     if ((uVar2 & 0x800) != 0) goto LAB_8005cd78;
//     if ((uVar2 & 0x1000) != 0) {
//       local_18 = local_18 + -1;
//     }
//     if ((uVar2 & 0x4000) != 0) {
//       local_18 = local_18 + 1;
//     }
//     if ((uVar2 & 0x2000) != 0) {
//       local_10 = local_10 + -1;
//     }
//     if ((uVar2 & 0x8000) != 0) {
//       local_10 = local_10 + 1;
//     }
//     if ((uVar2 & 4) != 0) {
//       local_14 = local_14 + -1;
//     }
//     if ((uVar2 & 1) != 0) {
//       local_14 = local_14 + 1;
//     }
//   }
//   if ((uVar2 & 0x800) != 0) {
// LAB_8005cd78:
//     iVar5 = (int)local_18 * 1000;
//     iVar6 = (int)local_10 * 1000;
//     lVar3 = GetAreaMapLevel(GlobalAreaMap,iVar5,(int)local_14 * 1000);
//     pHVar1 = StagePlayer;
//     if (lVar3 != -0x80000000) {
//       StagePlayer->locate->vx = iVar5;
//       ViewInfo.vpx = iVar5;
//       ViewInfo.vrx = iVar5;
//       pHVar1->locate->vy = lVar3;
//       ViewInfo.vpy = lVar3;
//       ViewInfo.vry = lVar3;
//       pHVar1->locate->vz = iVar6;
//       ViewInfo.vpz = iVar6;
//       ViewInfo.vrz = iVar6;
//       GsSetRefView2(&ViewInfo);
//     }
//   }
//   do {
//     iVar5 = GetRealPad(0);
//   } while (iVar5 != 0);
//   StartDrawing();
//   return;
// }
