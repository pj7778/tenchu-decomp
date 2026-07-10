#include "common.h"
#include "main.exe.h"

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
