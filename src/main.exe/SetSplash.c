#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void SetSplash(struct VECTOR *pos, short sx, short sy, int speed);
 *     EFFECT.C:1023, 17 src lines, frame 0 bytes, saved-reg mask 0x00000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct VECTOR * pos
 *     param $a1       short sx
 *     param $a2       short sy
 *     param $a3       int speed
 *
 * Globals it touches, as the original declared them:
 *     extern int Projection;
 *     extern struct tag_EffectSlot EffectSlot[200];
 *     extern struct Humanoid *HumanGroup[32];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetSplash", SetSplash);

// triage: EASY — 49 insns, 1 loop, 0 callees, ~0.08 to ReqItemManebue
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetSplash(VECTOR *pos,short sx,short sy,int speed)
//
// {
//   int iVar1;
//   long lVar2;
//   tag_EffectSlot *ptVar3;
//   int iVar4;
//
//   iVar4 = 0;
//   ptVar3 = EffectSlot + DAT_80097a3c;
//   iVar1 = DAT_80097a3c;
//   while( true ) {
//     iVar1 = iVar1 + 1;
//     ptVar3 = ptVar3 + 1;
//     if (199 < iVar1) {
//       iVar1 = 0;
//       ptVar3 = EffectSlot;
//     }
//     if (ptVar3->proc == (undefined **)0x0) break;
//     iVar4 = iVar4 + 1;
//     if (199 < iVar4) {
//       ptVar3 = &dmy;
// LAB_80039318:
//       (ptVar3->param).blood.hint = (AreaNodeType *)pos->vx;
//       (ptVar3->param).blood.px = pos->vy;
//       lVar2 = pos->vz;
//       (ptVar3->param).splash.mode = '\0';
//       (ptVar3->param).splash.sx = sx;
//       (ptVar3->param).splash.sy = sy;
//       (ptVar3->param).splash.speed = (uchar)speed;
//       (ptVar3->param).blood.py = lVar2;
//       ptVar3->proc = (undefined **)DrawSplash;
//       return;
//     }
//   }
//   DAT_80097a3c = iVar1 + 1;
//   if (199 < iVar1 + 1) {
//     DAT_80097a3c = 0;
//   }
//   goto LAB_80039318;
// }
