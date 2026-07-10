#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/think_setting_small_rotation_small_steps_", think_setting_small_rotation_small_steps_);

// triage: HARD — 348 insns, mul/div, 8 callees, ~0.05 to ProcItemKusuri
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Removing unreachable block (ram,0x8002cb78) */
//
// int FUN_8002c86c(void)
//
// {
//   undefined **ppuVar1;
//   uchar uVar2;
//   short sVar3;
//   long lVar4;
//   Humanoid *human;
//   uint uVar5;
//   VECTOR *pVVar6;
//   short sVar7;
//   int iVar8;
//   int iVar9;
//   uint uVar10;
//
//   uVar10 = 0;
//   iVar9 = Me_THINK_C->chase[0] - Me_THINK_C->locate->vx;
//   iVar8 = Me_THINK_C->chase[1] - Me_THINK_C->locate->vz;
//   if (Me_THINK_C->actscnt == '\0') {
//     uVar10 = FUN_8002b990(iVar9,iVar8);
//     lVar4 = SquareRoot0(iVar9 * iVar9 + iVar8 * iVar8);
//     if ((1999 < lVar4) && (iVar8 = uVar10 << 0x10, (Attrib & 0x400U) == 0)) goto LAB_8002cdbc;
//     DAT_800979c0 = 300;
//     if (PersistentState._88_1_ == '\x02') {
//       DAT_800979c0 = 600;
//     }
//     Sound(Me_THINK_C,0xe);
//     if (PersistentState._88_1_ == '\x01') {
//       iVar8 = rand();
//       Me_THINK_C->actcnt = (byte)iVar8 & 1;
//     }
//     else if ((byte)PersistentState._88_1_ < 2) {
//       if (PersistentState._88_1_ == '\0') {
//         Me_THINK_C->actcnt = '\x01';
//       }
//     }
//     else if (PersistentState._88_1_ == '\x02') {
//       Me_THINK_C->actcnt = '\0';
//     }
//     uVar2 = '\x01';
//     if ((Me_THINK_C->actcnt == '\0') && ((0x1d < Humans || (uVar2 = '\x02', StageID == 8)))) {
//       uVar2 = '\x01';
//     }
//     Me_THINK_C->actscnt = uVar2;
//   }
//   else if (Me_THINK_C->actscnt == '\x01') {
//     Me_THINK_C->actcnt = Me_THINK_C->actcnt + 1 & 0x1f;
//     if ((Me_THINK_C->actcnt & 8) != 0) {
//       if (Me_THINK_C->actcnt != 8) {
//         iVar8 = (uint)(Me_THINK_C->pad).data << 0x10;
//         goto LAB_8002cdbc;
//       }
//       iVar9 = (int)Degree;
//       iVar8 = iVar9;
//       if (iVar9 < 0) {
//         iVar8 = -iVar9;
//       }
//       if (iVar8 < 0x2bd) {
//         iVar8 = rand();
//         uVar10 = 0x1000;
//         if (iVar8 != (iVar8 / 5) * 5) {
//           uVar10 = rand();
//           iVar8 = 0x20000000;
//           if ((uVar10 & 1) == 0) goto LAB_8002cdbc;
//           uVar10 = 0xffff8000;
//         }
//       }
//       else {
//         uVar10 = 0xffff8000;
//         if (0 < iVar9) {
//           uVar10 = 0x2000;
//         }
//       }
//     }
//   }
//   else {
//     sVar3 = GetDirection(iVar9,iVar8,Me_THINK_C->rotate->vy);
//     iVar8 = (int)sVar3;
//     uVar5 = 0x2000;
//     if (0 < iVar8) {
//       uVar5 = 0x8000;
//     }
//     if (iVar8 < 0) {
//       iVar8 = -iVar8;
//     }
//     uVar10 = uVar5 | 0x4000;
//     if (999 < iVar8) {
//       uVar10 = uVar5 | 0x1000;
//     }
//     if ((Attrib & 0x400U) != 0) {
//       iVar9 = (int)Degree;
//       iVar8 = iVar9;
//       if (iVar9 < 0) {
//         iVar8 = -iVar9;
//       }
//       if ((1000 < iVar8) && (Me_THINK_C->field40_0xb0 == (undefined *)0x0)) {
//         if (Me_THINK_C->turn == 0) {
//           trap(0x1c00);
//         }
//         uVar5 = 0x20000000;
//         if (0 < iVar9) {
//           uVar5 = 0x80000000;
//         }
//         Me_THINK_C->field40_0xb0 = (undefined *)(1000 / (int)Me_THINK_C->turn | uVar5);
//       }
//     }
//     iVar8 = uVar10 << 0x10;
//     if (Distance < 0x4075) goto LAB_8002cdbc;
//     DAT_800979c0 = 300;
//     if (PersistentState._88_1_ == '\x02') {
//       DAT_800979c0 = 600;
//     }
//     Me_THINK_C->actscnt = '\0';
//     Me_THINK_C->actcnt = '\x01';
//     uVar5 = rand();
//     sVar7 = 10;
//     if ((uVar5 & 1) != 0) {
//       sVar7 = 9;
//     }
//     Sound(Me_THINK_C,sVar7);
//     iVar8 = rand();
//     sVar7 = AIDHumanType[StageID * 2 + iVar8 % 2];
//     pVVar6 = Me_THINK_C->locate;
//     sVar3 = Me_THINK_C->rotate->vy + sVar3;
//     Me_THINK_C->rotate->vy = sVar3;
//     human = (Humanoid *)BreedLife((int)sVar7,pVVar6->vx,pVVar6->vy,pVVar6->vz,(int)sVar3);
//     human->target = Me_THINK_C->target;
//     human->think[0] = Think1Func[4];
//     human->think[1] = Think2Func[4];
//     human->think[2] = Think3Func[4];
//     ppuVar1 = Think4Func[4];
//     human->attribute = human->attribute | 4;
//     human->think[3] = ppuVar1;
//     EquipWeapon(human,1);
//     SetNowMotion(human,0x501,1);
//     human->actscnt = '\0';
//     human->actcnt = '\x01';
//     human->attribute = human->attribute | 0x11;
//     iVar8 = rand();
//     human->chase[0] = Me_THINK_C->chase[0] + (iVar8 % 5 + -2) * 500;
//     iVar8 = rand();
//     human->chase[1] = Me_THINK_C->chase[1] + (iVar8 % 5 + -2) * 500;
//     uVar5 = rand();
//     sVar3 = 10;
//     if ((uVar5 & 1) != 0) {
//       sVar3 = 9;
//     }
//     Sound(human,sVar3);
//     StageEnemies = StageEnemies + 1;
//   }
//   iVar8 = uVar10 << 0x10;
// LAB_8002cdbc:
//   return iVar8 >> 0x10;
// }
