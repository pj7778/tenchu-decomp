#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/CreateStage", CreateStage);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/CreateStage", initialise_stage_and_character__override__prt_8003a3dc_e0d8bb81);

// triage: HARD — 249 insns, mul/div, 1 loop, 31 callees, ~0.09 to AddMisc
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
//
// void CreateStage(int StageNo,int CharType)
//
// {
//   ulong *pt;
//   GsIMAGE *pGVar1;
//   BackGround *bg;
//   Humanoid *human;
//   int iVar2;
//   Humanoid *pHVar3;
//   int iVar4;
//   short stage;
//   short mode;
//   undefined1 auStack_78 [72];
//   undefined *local_30 [4];
//
//   if ((uint)StageNo < 0xb) {
//     SetDepthQ(0xffff810c,0x2f282e0);
//     _DepthPoint = 0x4e2;
//     while (pHVar3 = HumanGroup[0], mode = (short)CharType, 0 < Humans) {
//       DestroyTraceLine(HumanGroup[0]->trace);
//       KillHumanoid(pHVar3);
//     }
//     ImagePath = StageConfig[StageNo].path;
//     stage = (short)((uint)((StageNo + 1) * 0x10000) >> 0x10);
//     StageID = StageNo;
//     SetupSoundEffect(mode,stage);
//     DoBriefingAndInventorySelection();
//     local_30[0] = PTR_s_title_e_tim_80012090;
//     local_30[1] = PTR_s_title_f_tim_80012094;
//     local_30[2] = PTR_s_title_i_tim_80012098;
//     local_30[3] = PTR_s_title_j_tim_8001209c;
//     pt = PathFileRead(ImagePath,local_30[(byte)PersistentState._94_1_]);
//     pGVar1 = GetImage(0x2d);
//     FUN_8004eaf0(pGVar1,auStack_78,0x34,0x43);
//     bg = (BackGround *)FUN_8004f4f8(pt);
//     vfree((undefined *)pt);
//     FUN_80038ce0();
//     StartDrawing();
//     GsSortPoly(auStack_78,OTablePt,0);
//     DrawBG(bg);
//     SkipFrame = 2;
//     EndDrawing(0);
//     StartDrawing();
//     SkipFrame = 2;
//     EndDrawing(0);
//     DisposeBG(bg);
//     SetupAppearance(mode,stage);
//     PathFileRead(ImagePath,(uchar *)"STAGE.CON");
//     LoadConstruction();
//     FUN_80057698();
//     InitializeImage();
//     ResetInfoview(StageNo);
//     human = (Humanoid *)BreedLife((int)mode,0,0,0,0);
//     SetupThinkFunction(human,0x1111);
//     (human->model->locate).coord.t[0] = StageConfig[StageNo].px;
//     (human->model->locate).coord.t[1] = StageConfig[StageNo].py;
//     (human->model->locate).coord.t[2] = StageConfig[StageNo].pz;
//     (human->model->rotate).vx = 0;
//     (human->model->rotate).vy = (short)StageConfig[StageNo].pr;
//     (human->model->rotate).vz = 0;
//     pHVar3 = human;
//     iVar2 = 0;
//     CamState.Owner = human;
//     do {
//       iVar4 = iVar2 + 1;
//       pHVar3->item[0] = (&PersistentState.field_0x7)[iVar2];
//       pHVar3 = (Humanoid *)((int)(human->pad).stream + iVar2 + -0x17);
//       iVar2 = iVar4;
//     } while (iVar4 < 0x14);
//     FUN_8003d528((int)mode,StageNo);
//     if ((byte)PersistentState._6_1_ < 3) {
//       SystemFlag = SystemFlag & 0xfffffff7;
//     }
//     else {
//       iVar2 = rand();
//       PersistentState._6_1_ =
//            (char)iVar2 +
//            ((char)((ulonglong)((longlong)iVar2 * 0x55555556) >> 0x20) - (char)(iVar2 >> 0x1f)) * -3;
//       SystemFlag = SystemFlag | 8;
//     }
//     FUN_8003cc78(PersistentState._6_1_);
//     leLayoutEnemy(1);
//     ViewInfo.vpx = StageConfig[StageNo].px;
//     ViewInfo.vry = StageConfig[StageNo].py;
//     ViewInfo.vpz = StageConfig[StageNo].pz;
//     ViewInfo.vpy = ViewInfo.vry + -10000;
//     ViewInfo.vrx = ViewInfo.vpx;
//     ViewInfo.vrz = ViewInfo.vpz;
//     StartDrawing();
//     CVAsetup();
//     SetupStageSequence();
//     PadProc();
//     EndDrawing(0);
//   }
//   else {
//     AdtMessageBox("illigal stage id %d",StageNo);
//   }
//   return;
// }
