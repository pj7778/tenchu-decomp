#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawExplosion", DrawExplosion);

// triage: MEDIUM — 117 insns, mul/div, 2 callees, ~0.04 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void DrawExplosion(tag_EffectSlot *ef)
//
// {
//   byte bVar1;
//   short sVar2;
//   short sVar3;
//   long lVar4;
//   uchar uVar5;
//   uchar uVar6;
//   Sprite3D *unaff_s0;
//
//   bVar1 = (ef->param).blood.bright;
//   uVar6 = 0x80;
//   uVar5 = 0x80;
//   if (bVar1 == 1) {
//     if ((ef->param).blood.mode == '\0') {
//       (ef->param).blood.mode = '\x05';
//       (ef->param).blood.bright = (ef->param).blood.bright + '\x01';
//     }
//     (ef->param).smoke.scale = (ef->param).smoke.scale + 0x2000;
//     (ef->param).smoke.rotate = (ef->param).smoke.rotate + 0x64000;
//     unaff_s0 = sprBomb[1];
//   }
//   else {
//     uVar6 = uVar5;
//     if (bVar1 < 2) {
//       if (bVar1 == 0) {
//         if ((ef->param).blood.mode == '\0') {
//           (ef->param).blood.mode = '\x03';
//           (ef->param).blood.bright = (ef->param).blood.bright + '\x01';
//           unaff_s0 = sprBomb[0];
//         }
//         else {
//           (ef->param).smoke.scale = (ef->param).smoke.scale + 0x2000;
//           (ef->param).smoke.rotate = (ef->param).smoke.rotate + 0x64000;
//           unaff_s0 = sprBomb[0];
//         }
//       }
//     }
//     else if (bVar1 == 2) {
//       (ef->param).smoke.scale = (ef->param).smoke.scale + -0x333;
//       (ef->param).smoke.rotate = (ef->param).smoke.rotate + 0x5a000;
//       uVar6 = (uchar)(uint)((ulonglong)
//                             ((longlong)(int)((uint)(ef->param).blood.mode << 7) * 0x66666667) >>
//                            0x21);
//       unaff_s0 = sprBomb[1];
//       if ((ef->param).blood.mode == '\0') {
//         ef->proc = (undefined **)0x0;
//         unaff_s0 = sprBomb[1];
//       }
//     }
//   }
//   sVar2 = (ef->param).smoke.vec.vx;
//   sVar3 = (ef->param).smoke.vec.vy;
//   (ef->param).blood.mode = (ef->param).blood.mode + 0xff;
//   (ef->param).blood.pz = (ef->param).blood.pz + (int)sVar3;
//   sVar3 = (ef->param).smoke.vec.vz;
//   (ef->param).blood.py = (ef->param).blood.py + (int)sVar2;
//   (ef->param).blood.scale = (ef->param).blood.scale + (int)sVar3;
//   (unaff_s0->locate).coord.t[0] = (ef->param).blood.py;
//   (unaff_s0->locate).coord.t[1] = (ef->param).blood.pz;
//   (unaff_s0->locate).coord.t[2] = (ef->param).blood.scale;
//   unaff_s0->scale = (ef->param).smoke.scale;
//   lVar4 = (ef->param).smoke.rotate;
//   (unaff_s0->sprite).r = uVar6;
//   (unaff_s0->sprite).g = uVar6;
//   (unaff_s0->sprite).b = uVar6;
//   (unaff_s0->sprite).rotate = lVar4;
//   UpdateCoordinate((ModelType *)unaff_s0);
//   DrawSprite(unaff_s0);
//   return;
// }
