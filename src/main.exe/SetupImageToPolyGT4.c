#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetupImageToPolyGT4", SetupImageToPolyGT4);

// triage: TRIVIAL — 81 insns, 3 callees, NEAR-CLONE of SetupImageToPolyFT4 — clone it

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetupImageToPolyGT4(GsIMAGE *image,POLY_GT4 *ply,short x,short y)
//
// {
//   ushort uVar1;
//   ushort uVar2;
//   byte bVar3;
//   u_short uVar4;
//   u_char uVar5;
//   int iVar6;
//   u_char uVar7;
//   uint tp;
//   short sVar8;
//   short sVar9;
//
//   SetPolyGT4(ply);
//   tp = (ushort)image->pmode & 3;
//   uVar4 = GetTPage(tp,1,(int)image->px,(int)image->py);
//   ply->tpage = uVar4;
//   uVar4 = GetClut((int)image->cx,(int)image->cy);
//   ply->clut = uVar4;
//   sVar8 = image->px;
//   uVar7 = (u_char)image->py;
//   uVar1 = image->pw;
//   uVar2 = image->ph;
//   ply->r0 = '\x7f';
//                     /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
//                        setShadeTex(sprt16, 1) */
//   ply->g0 = '\x7f';
//   ply->b0 = '\x7f';
//   ply->r1 = '\x7f';
//                     /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
//                        setShadeTex(sprt16, 1) */
//   ply->g1 = '\x7f';
//   ply->b1 = '\x7f';
//   ply->r2 = '\x7f';
//                     /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
//                        setShadeTex(sprt16, 1) */
//   ply->g2 = '\x7f';
//   ply->b2 = '\x7f';
//   ply->r3 = '\x7f';
//                     /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
//                        setShadeTex(sprt16, 1) */
//   ply->g3 = '\x7f';
//   ply->b3 = '\x7f';
//   ply->x0 = x;
//   ply->y0 = y;
//   ply->y1 = y;
//   ply->x2 = x;
//   bVar3 = (byte)((int)sVar8 << (2 - tp & 0x1f)) & (char)(1 << (8 - tp & 0x1f)) - 1U;
//   iVar6 = (uint)uVar1 << (2 - tp & 0x1f);
//   sVar8 = x + (short)iVar6;
//   sVar9 = y + uVar2;
//   uVar5 = bVar3 + (char)iVar6;
//   ply->_2 = uVar7;
//   ply->_3 = uVar7;
//   uVar7 = uVar7 + (char)uVar2;
//   ply->x1 = sVar8;
//   ply->y2 = sVar9;
//   ply->x3 = sVar8;
//   ply->y3 = sVar9;
//   ply->u0 = bVar3;
//   ply->u1 = uVar5;
//   ply->u2 = bVar3;
//   ply->v2 = uVar7;
//   ply->u3 = uVar5;
//   ply->v3 = uVar7;
//   return;
// }
