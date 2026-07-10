#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/KillHumanoid", KillHumanoid);

// triage: EASY — 63 insns, 1 loop, 6 callees, ~0.15 to DisposeBG
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void KillHumanoid(Humanoid *human)
//
// {
//   ushort uVar1;
//   int iVar2;
//   short sVar3;
//   int iVar4;
//
//   if (human != (Humanoid *)0x0) {
//     DeleteConflict(*human->model->object);
//     DisposeModelArchive(human->model);
//     DisposeMotionManager(human->motion);
//     DisposeWeapon(human);
//     FUN_800270c8(human,3);
//     vfree((undefined *)human);
//     iVar4 = 0;
//     if (0 < Humans) {
//       iVar2 = 0;
//       do {
//         sVar3 = (short)iVar4;
//         iVar4 = iVar4 + 1;
//         if (*(Humanoid **)((int)HumanGroup + (iVar2 >> 0xe)) == human) break;
//         iVar2 = iVar4 * 0x10000;
//         sVar3 = (short)iVar4;
//       } while (iVar4 * 0x10000 >> 0x10 < (int)Humans);
//       uVar1 = Humans - 1;
//       if ((int)sVar3 < (int)Humans) {
//         Humans = uVar1;
//         HumanGroup[sVar3] = *(Humanoid **)((int)HumanGroup + ((int)((uint)uVar1 << 0x10) >> 0xe));
//       }
//     }
//   }
//   return;
// }
