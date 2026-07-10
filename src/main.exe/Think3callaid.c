#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/Think3callaid", Think3callaid);

// triage: EASY — 102 insns, 6 callees, ~0.03 to ProcItemGoshikimai
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// short Think3callaid(void)
//
// {
//   ushort uVar1;
//   undefined **ppuVar2;
//   Humanoid *human;
//   short sVar3;
//   int iVar4;
//   Humanoid *human_00;
//   VECTOR *pVVar5;
//
//   if (Distance < 0x4074) {
//     if (SR != -2) {
//       SR = 0;
//     }
//     sVar3 = Think3escape();
//   }
//   else {
//     SR = -1;
//     iVar4 = rand();
//     pVVar5 = Me_THINK_C->locate;
//     human_00 = (Humanoid *)
//                BreedLife((int)AIDHumanType[StageID * 2 + iVar4 % 2],pVVar5->vx,pVVar5->vy,pVVar5->vz
//                          ,(int)Me_THINK_C->rotate->vy + (int)Degree);
//     human = Me_THINK_C;
//     human_00->target = Me_THINK_C->target;
//     KillHumanoid(human);
//     human_00->think[0] = Think1Func[4];
//     human_00->think[1] = Think2Func[4];
//     Pad = &human_00->pad;
//     uVar1 = human_00->attribute;
//     Me_THINK_C = human_00;
//     human_00->think[2] = Think3Func[4];
//     ppuVar2 = Think4Func[4];
//     human_00->attribute = uVar1 | 4;
//     human_00->think[3] = ppuVar2;
//     EquipWeapon(human_00,1);
//     SetNowMotion(Me_THINK_C,0x501,1);
//     Attrib = Me_THINK_C->attribute | 2;
//     sVar3 = 0;
//     if ((Me_THINK_C->type & 0xf0U) == 0x90) {
//       StageEnemies = StageEnemies + 1;
//       StageCitizens = StageCitizens + -1;
//       sVar3 = 0;
//     }
//   }
//   return sVar3;
// }
