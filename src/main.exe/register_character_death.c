#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/register_character_death", register_character_death);

// triage: HARD — 205 insns, mul/div, 3 loop, 5 callees, ~0.05 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Loops: 3 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8002bcb8(int param_1)
//
// {
//   short sVar1;
//   long lVar2;
//   VECTOR *pVVar3;
//   int iVar4;
//   Humanoid *human;
//   short n;
//   int iVar5;
//   int local_28;
//   int local_24;
//   int local_20;
//   SVECTOR local_18;
//
//   n = 1;
//   if (((*(ushort *)(param_1 + 4) & 0x10) == 0) && (PersistentState._88_1_ != '\0')) {
//     iVar4 = (int)Humans;
//     iVar5 = (int)(short)(DAT_800979de + 1) % iVar4;
//     if (iVar4 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar4 == -1) && ((short)(DAT_800979de + 1) == -0x80000000)) {
//       trap(0x1800);
//     }
//     human = *(Humanoid **)((int)HumanGroup + ((iVar5 << 0x10) >> 0xe));
//     DAT_800979de = (short)iVar5;
//     if (((((human->attribute & 0x83U) == 0) && (human->status != 0x11)) && (human->status != 0x10))
//        && (human != StagePlayer)) {
//       local_28 = **(int **)(param_1 + 0x38) - human->locate->vx;
//       local_20 = *(int *)(*(int *)(param_1 + 0x38) + 8) - human->locate->vz;
//       sVar1 = GetDirection(local_28,local_20,human->rotate->vy);
//       iVar4 = (int)sVar1;
//       if (iVar4 < 0) {
//         iVar4 = -iVar4;
//       }
//       if ((iVar4 < 0x385) &&
//          (lVar2 = SquareRoot0(local_28 * local_28 + local_20 * local_20), lVar2 < 0x4e21)) {
//         local_24 = (*(int *)(*(int *)(param_1 + 0x38) + 4) - human->locate->vy) - (int)human->height
//         ;
//         iVar4 = local_28;
//         if (local_28 < 0) {
//           iVar4 = -local_28;
//         }
//         if (500 < iVar4) goto LAB_8002beac;
//         iVar4 = local_24;
//         if (local_24 < 0) {
//           iVar4 = -local_24;
//         }
//         if (500 < iVar4) goto LAB_8002beac;
//         n = 1;
//         iVar4 = local_20;
//         if (local_20 < 0) {
//           iVar4 = -local_20;
//         }
//         while (500 < iVar4) {
// LAB_8002beac:
//           do {
//             do {
//               n = n << 1;
//               local_28 = local_28 >> 1;
//               local_24 = local_24 >> 1;
//               local_20 = local_20 >> 1;
//               iVar4 = local_28;
//               if (local_28 < 0) {
//                 iVar4 = -local_28;
//               }
//             } while (500 < iVar4);
//             iVar4 = local_24;
//             if (local_24 < 0) {
//               iVar4 = -local_24;
//             }
//           } while (500 < iVar4);
//           iVar4 = local_20;
//           if (local_20 < 0) {
//             iVar4 = -local_20;
//           }
//         }
//         local_18.vx = (short)local_28;
//         local_18.vy = (short)local_24;
//         local_18.vz = (short)local_20;
//         pVVar3 = GetAreaMapPassage(GlobalAreaMap,human->locate,&local_18,n);
//         if (pVVar3 == (VECTOR *)0x0) {
//           DAT_800979c0 = 300;
//           if (PersistentState._88_1_ == '\x02') {
//             DAT_800979c0 = 600;
//           }
//           Sound(human,0xc);
//           SetNowMotion(human,0x80e,1);
//           *(ushort *)(param_1 + 4) = *(ushort *)(param_1 + 4) | 0x10;
//           human->attribute = human->attribute | 0x11;
//           human->chase[0] = **(long **)(param_1 + 0x38);
//           lVar2 = *(long *)(*(int *)(param_1 + 0x38) + 8);
//           human->actcnt = '\0';
//           human->actscnt = '\0';
//           human->chase[1] = lVar2;
//         }
//       }
//     }
//   }
//   return;
// }
