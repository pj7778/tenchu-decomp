#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ProcMiscFire", ProcMiscFire);

// triage: MEDIUM — 89 insns, mul/div, 5 callees, ~0.09 to SetupAfterimage
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void ProcMiscFire(tag_TMisc *m,TMiscMessage msg)
//
// {
//   int iVar1;
//   SVECTOR local_28;
//   VECTOR local_20;
//
//   if (msg == MM_CREATE) {
//     m->mode = '\0';
//     m->count = 10;
//   }
//   else if (((MM_RESUME < msg) && (m->mode == '\0')) &&
//           (iVar1 = m->count + -1, m->count = iVar1, iVar1 < 1)) {
//     local_28.vx = (short)DAT_80097c48;
//     local_28.vy = DAT_80097c48._2_2_;
//     local_28.vz = (short)DAT_80097c4c;
//     local_28.pad = DAT_80097c4c._2_2_;
//     local_20.vx = m->x;
//     local_20.vy = m->y;
//     local_20.vz = m->z;
//     SetExplosion(&local_20,&local_28);
//     local_28.vx = 0x4b;
//     local_28.vy = 0xb4;
//     local_28.vz = 0x4b;
//     SetHinoko(&local_20,&local_28,10);
//     local_28.vx = 0;
//     local_28.vy = -200;
//     local_28._4_4_ = (uint)(ushort)local_28.pad << 0x10;
//     SetSmoke(&local_20,&local_28,0x14,6);
//     iVar1 = rand();
//     m->count = iVar1 % 0x96;
//     SoundEx(&local_20,0x28);
//   }
//   return;
// }
