#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/SetupAppearance", SetupAppearance);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/SetupAppearance", load_chosen_player_character_resources___override__prt_80029d94_407e704c);

// triage: HARD — 256 insns, 3 loop, 6 callees, ~0.06 to SetupMotionRegist
// likely-relevant cookbook sections:
//   - Loops: 3 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetupAppearance(short mode,short stage)
//
// {
//   char *pcVar1;
//   int iVar2;
//   ulong *puVar3;
//   short sVar4;
//   short sVar5;
//   MotionRegistType *pMVar6;
//   int iVar7;
//   int iVar8;
//   char acStack_90 [104];
//
//   EngageLevel = 3 - (ushort)(byte)PersistentState._88_1_;
//   if (PersistentState._26_1_ != '\0') {
//     HumanData[0].name = "RIKIMAUA";
//     if (PersistentState._26_1_ == -1) {
//       pcVar1 = s_AYAMES_800979b0;
//     }
//     else {
//       pcVar1 = s_AYAMEA_800979a8;
//     }
//     PersistentState._26_1_ = 0;
//     DAT_800979a2 = 0xffff;
//     HumanData[1].name = (uchar *)pcVar1;
//   }
//   iVar8 = 0;
//   NowStage = stage;
//   sVar4 = HumanData[0].type;
//   while (sVar4 != -1) {
//     sVar4 = (short)iVar8;
//     if (HumanData[sVar4].model != (ulong *)0x0) {
//       iVar7 = 0;
//       vfree((undefined *)HumanData[sVar4].model);
//       HumanData[sVar4].model = (ulong *)0x0;
//       sVar5 = HumanData[0].type;
//       while (sVar5 != -1) {
//         sVar5 = (short)iVar7;
//         iVar2 = strcmp((char *)HumanData[sVar4].name,(char *)HumanData[sVar5].name);
//         iVar7 = iVar7 + 1;
//         if (iVar2 == 0) {
//           HumanData[sVar5].model = (ulong *)0x0;
//         }
//         sVar5 = HumanData[iVar7 * 0x10000 >> 0x10].type;
//       }
//     }
//     pMVar6 = HumanData[sVar4].mtbl;
//     iVar8 = iVar8 + 1;
//     HumanData[sVar4].model = (ulong *)0x0;
//     pMVar6->motion = (MotionDataType *)0x0;
//     sVar4 = HumanData[iVar8 * 0x10000 >> 0x10].type;
//   }
//   iVar8 = 0;
//   if (WeaponModel[0].wid != -1) {
//     iVar7 = 0;
//     do {
//       if (WeaponModel[iVar7 >> 0x10].model != (ulong *)0x0) {
//         vfree((undefined *)WeaponModel[iVar7 >> 0x10].model);
//       }
//       iVar8 = iVar8 + 1;
//       WeaponModel[iVar7 >> 0x10].model = (ulong *)0x0;
//       iVar7 = iVar8 * 0x10000;
//     } while (WeaponModel[iVar8 * 0x10000 >> 0x10].wid != -1);
//   }
//   iVar8 = (int)stage;
//   if (iVar8 < 0) {
//     if (CommonMotion != (MotionPackType *)0x0) {
//       vfree((undefined *)CommonMotion);
//       CommonMotion = (MotionPackType *)0x0;
//     }
//     if (PlayerMotion != (MotionPackType *)0x0) {
//       vfree((undefined *)PlayerMotion);
//       PlayerMotion = (MotionPackType *)0x0;
//     }
//     if (StageMotion != (MotionPackType *)0x0) {
//       vfree((undefined *)StageMotion);
//       StageMotion = (MotionPackType *)0x0;
//     }
//   }
//   else {
//     if (StageMotion != (MotionPackType *)0x0) {
//       vfree((undefined *)StageMotion);
//     }
//     DAT_800979a6 = stage;
//     sprintf(acStack_90,"%sMOTION\\STAGE%d.AMD","K:\\WORK\\CDIMAGE\\HUMAN\\",iVar8);
//     puVar3 = FileRead(acStack_90);
//     StageMotion = LoadMotion(puVar3);
//     if (iVar8 != 0) {
//       if (CommonMotion == (MotionPackType *)0x0) {
//         puVar3 = FileRead("K:\\WORK\\CDIMAGE\\HUMAN\\MOTION\\COMMON.AMD");
//         CommonMotion = LoadMotion(puVar3);
//         SetupMotionRegist(MOTcommon);
//       }
//       if (PlayerMotion != (MotionPackType *)0x0) {
//         if (mode != DAT_800979a4) {
//           vfree((undefined *)PlayerMotion);
//           PlayerMotion = (MotionPackType *)0x0;
//         }
//         if (PlayerMotion != (MotionPackType *)0x0) {
//           return;
//         }
//       }
//       if (mode == 0) {
//         pcVar1 = "K:\\WORK\\CDIMAGE\\HUMAN\\MOTION\\RIKIMARU.AMD";
//       }
//       else {
//         pcVar1 = "K:\\WORK\\CDIMAGE\\HUMAN\\MOTION\\AYAME.AMD";
//       }
//       DAT_800979a4 = mode;
//       puVar3 = FileRead(pcVar1);
//       PlayerMotion = LoadMotion(puVar3);
//     }
//   }
//   return;
// }
