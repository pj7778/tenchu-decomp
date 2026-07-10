#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void SetBleed(struct VECTOR *pos, struct SVECTOR *vec, int time, long col);
 *     EFFECT.C:946, 14 src lines, frame 0 bytes, saved-reg mask 0x00000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct VECTOR * pos
 *     param $a1       struct SVECTOR * vec
 *     param $a2       int time
 *     param $a3       long col
 *
 * Globals it touches, as the original declared them:
 *     extern int Projection;
 *     extern struct tag_EffectSlot EffectSlot[200];
 *     extern struct Humanoid *HumanGroup[32];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetBleed", SetBleed);

// triage: EASY — 61 insns, 1 loop, 0 callees, ~0.11 to InsertConflict
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetBleed(VECTOR *pos,SVECTOR *vec,int time,long col)
//
// {
//   int iVar1;
//   tag_EffectSlot *ptVar2;
//   int iVar3;
//   long lVar4;
//   long lVar5;
//   long lVar6;
//
//   iVar3 = 0;
//   ptVar2 = EffectSlot + DAT_80097a3c;
//   iVar1 = DAT_80097a3c;
//   while( true ) {
//     iVar1 = iVar1 + 1;
//     ptVar2 = ptVar2 + 1;
//     if (199 < iVar1) {
//       iVar1 = 0;
//       ptVar2 = EffectSlot;
//     }
//     if (ptVar2->proc == (undefined **)0x0) break;
//     iVar3 = iVar3 + 1;
//     if (199 < iVar3) {
//       ptVar2 = &dmy;
// LAB_800393e0:
//       lVar4 = pos->vy;
//       lVar5 = pos->vz;
//       lVar6 = pos->pad;
//       (ptVar2->param).blood.hint = (AreaNodeType *)pos->vx;
//       (ptVar2->param).blood.px = lVar4;
//       (ptVar2->param).blood.py = lVar5;
//       (ptVar2->param).blood.pz = lVar6;
//       lVar4 = *(long *)&vec->vz;
//       (ptVar2->param).blood.scale = *(long *)vec;
//       (ptVar2->param).blood.rotate = lVar4;
//       (ptVar2->param).bleed.r = (uchar)((uint)col >> 0x10);
//       (ptVar2->param).bleed.g = (uchar)((uint)col >> 8);
//       (ptVar2->param).bleed.time = (uchar)time;
//       (ptVar2->param).bleed.b = (uchar)col;
//       (ptVar2->param).bleed.mode = '\0';
//       ptVar2->proc = (undefined **)DrawBleed;
//       return;
//     }
//   }
//   DAT_80097a3c = iVar1 + 1;
//   if (199 < iVar1 + 1) {
//     DAT_80097a3c = 0;
//   }
//   goto LAB_800393e0;
// }
