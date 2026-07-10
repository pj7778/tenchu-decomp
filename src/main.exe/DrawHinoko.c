#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * static void DrawHinoko(struct tag_EffectSlot *ef);
 *     EFFECT.C:1258, 61 src lines, frame 32 bytes, saved-reg mask 0x80010000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a1       struct tag_EffectSlot * ef
 *     reg   $a2       struct ExplosionType * param
 *     reg   $s0       struct Sprite3D * spr
 *     reg   $a3       unsigned char alfa
 *
 * Globals it touches, as the original declared them:
 *     extern struct Sprite3D *sprBomb[3];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawHinoko", DrawHinoko);

// triage: EASY — 77 insns, mul/div, 2 callees, ~0.05 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void DrawHinoko(tag_EffectSlot *ef)
//
// {
//   uchar uVar1;
//   short sVar2;
//   short sVar3;
//   short sVar4;
//   Sprite3D *sprt;
//   long lVar5;
//   int iVar6;
//   uint uVar7;
//   uchar uVar8;
//
//   sprt = sprBomb[2];
//   uVar1 = (ef->param).blood.bright;
//   uVar8 = 0x80;
//   if (uVar1 == '\0') {
//     if ((ef->param).blood.mode == '\0') {
//       (ef->param).blood.bright = '\x01';
//       (ef->param).blood.mode = '\x1e';
//     }
//   }
//   else if ((uVar1 == '\x01') &&
//           (uVar7 = (uint)(ef->param).blood.mode,
//           uVar8 = (uchar)(uint)((ulonglong)((longlong)(int)(uVar7 * 0x80) * 0x88888889) >> 0x24),
//           uVar7 == 0)) {
//     ef->proc = (undefined **)0x0;
//   }
//   sVar2 = (ef->param).smoke.vec.vx;
//   sVar3 = (ef->param).smoke.vec.vz;
//   (ef->param).blood.mode = (ef->param).blood.mode + 0xff;
//   sVar4 = (ef->param).smoke.vec.vy;
//   (ef->param).smoke.scale = (ef->param).smoke.scale + 0xc00;
//   iVar6 = (ef->param).blood.scale;
//   (ef->param).smoke.vec.vy = sVar4 + 5;
//   (ef->param).blood.scale = iVar6 + sVar3;
//   sVar3 = (ef->param).smoke.vec.vy;
//   (ef->param).blood.py = (ef->param).blood.py + (int)sVar2;
//   (ef->param).blood.pz = (ef->param).blood.pz + (int)sVar3;
//   (sprt->locate).coord.t[0] = (ef->param).blood.py;
//   (sprt->locate).coord.t[1] = (ef->param).blood.pz;
//   (sprt->locate).coord.t[2] = (ef->param).blood.scale;
//   sprt->scale = (ef->param).smoke.scale;
//   lVar5 = (ef->param).smoke.rotate;
//   (sprt->sprite).r = uVar8;
//   (sprt->sprite).g = uVar8;
//   (sprt->sprite).b = uVar8;
//   (sprt->sprite).rotate = lVar5;
//   UpdateCoordinate((ModelType *)sprt);
//   DrawSprite(sprt);
//   return;
// }
