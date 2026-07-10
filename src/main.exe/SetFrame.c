#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetFrame", SetFrame);

// triage: EASY — 49 insns, 1 loop, 0 callees, ~0.07 to ReqItemManebue
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetFrame(VECTOR *pos,short size,short time,_GsCOORDINATE2 *super)
//
// {
//   long lVar1;
//   int iVar2;
//   tag_EffectSlot *ptVar3;
//   int iVar4;
//
//   iVar4 = 0;
//   ptVar3 = EffectSlot + DAT_80097a3c;
//   iVar2 = DAT_80097a3c;
//   while( true ) {
//     iVar2 = iVar2 + 1;
//     ptVar3 = ptVar3 + 1;
//     if (199 < iVar2) {
//       iVar2 = 0;
//       ptVar3 = EffectSlot;
//     }
//     if (ptVar3->proc == (undefined **)0x0) break;
//     iVar4 = iVar4 + 1;
//     if (199 < iVar4) {
//       ptVar3 = &dmy;
// LAB_80039120:
//       (ptVar3->param).blood.px = pos->vx;
//       (ptVar3->param).blood.py = pos->vy;
//       lVar1 = pos->vz;
//       (ptVar3->param).frame.mode = '\0';
//       (ptVar3->param).bleed.vec.vx = size;
//       (ptVar3->param).bleed.vec.vy = time;
//       (ptVar3->param).blood.pz = lVar1;
//       (ptVar3->param).frame.super = super;
//       ptVar3->proc = (undefined **)DrawFrame;
//       return;
//     }
//   }
//   DAT_80097a3c = iVar2 + 1;
//   if (199 < iVar2 + 1) {
//     DAT_80097a3c = 0;
//   }
//   goto LAB_80039120;
// }
