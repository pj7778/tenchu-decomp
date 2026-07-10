#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * static void DrawPause(void);
 *     INFOVIEW.C:1224, 35 src lines, frame 304 bytes, saved-reg mask 0x80030000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     stack sp+16     struct DISPENV o_disp
 *     stack sp+40     struct DRAWENV o_draw
 *     stack sp+136    struct DRAWENV n_draw
 *     stack sp+232    struct POLY_GT4 ply
 *
 * Globals it touches, as the original declared them:
 *     extern enum TSystemFlag SystemFlag;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawPause", DrawPause);

// triage: MEDIUM — 114 insns, 1 loop, 7 callees, ~0.10 to InsertConflict
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Type propagation algorithm not settling */
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// void DrawPause(void)
//
// {
//   short sVar1;
//   short sVar2;
//   short sVar3;
//   short sVar4;
//   short sVar5;
//   u_short uVar6;
//   u_char uVar7;
//   u_char uVar8;
//   u_char uVar9;
//   u_char uVar10;
//   u_char uVar11;
//   DRAWENV *pDVar12;
//   DRAWENV *pDVar13;
//   GsIMAGE *image;
//   int iVar14;
//   short in_a0;
//   undefined4 uVar15;
//   u_char uVar16;
//   DRAWENV *pDVar17;
//   DRAWENV *pDVar18;
//   DISPENV local_120;
//   DRAWENV local_108;
//   DRAWENV local_a8;
//   POLY_GT4 PStack_48;
//
//   if ((SystemFlag & 1) == 0) {
//     GetDrawEnv(&local_108);
//     GetDispEnv(&local_120);
//     pDVar12 = &local_a8;
//     pDVar18 = &local_108;
//     do {
//       pDVar17 = pDVar18;
//       pDVar13 = pDVar12;
//       sVar1 = (pDVar17->clip).y;
//       sVar2 = (pDVar17->clip).w;
//       sVar3 = (pDVar17->clip).h;
//       uVar15 = *(undefined4 *)pDVar17->ofs;
//       sVar4 = (pDVar17->tw).x;
//       sVar5 = (pDVar17->tw).y;
//       (pDVar13->clip).x = (pDVar17->clip).x;
//       (pDVar13->clip).y = sVar1;
//       (pDVar13->clip).w = sVar2;
//       (pDVar13->clip).h = sVar3;
//       *(undefined4 *)pDVar13->ofs = uVar15;
//       (pDVar13->tw).x = sVar4;
//       (pDVar13->tw).y = sVar5;
//       pDVar18 = (DRAWENV *)&(pDVar17->tw).w;
//       pDVar12 = (DRAWENV *)&(pDVar13->tw).w;
//     } while (pDVar18 != (DRAWENV *)(local_108.dr_env.code + 0xc));
//     sVar1 = (pDVar17->tw).h;
//     uVar6 = pDVar17->tpage;
//     uVar16 = pDVar17->dtd;
//     uVar7 = pDVar17->dfe;
//     uVar8 = pDVar17->isbg;
//     uVar9 = pDVar17->r0;
//     uVar10 = pDVar17->g0;
//     uVar11 = pDVar17->b0;
//     (pDVar13->tw).w = (pDVar17->tw).w;
//     (pDVar13->tw).h = sVar1;
//     pDVar13->tpage = uVar6;
//     pDVar13->dtd = uVar16;
//     pDVar13->dfe = uVar7;
//     pDVar13->isbg = uVar8;
//     pDVar13->r0 = uVar9;
//     pDVar13->g0 = uVar10;
//     pDVar13->b0 = uVar11;
//     local_a8.clip.x = local_120.disp.x;
//     local_a8.clip.y = local_120.disp.y;
//     local_a8.clip.w = local_120.disp.w;
//     local_a8.clip.h = local_120.disp.h;
//     local_a8.ofs[0] = local_120.disp.x;
//     local_a8.ofs[1] = local_120.disp.y;
//     PutDrawEnv(&local_a8);
//     image = GetImage(0x3d);
//     SetupImageToPolyGT4(image,&PStack_48,image->pw * -2 + 0xa0,0x78 - (image->ph >> 1));
//     iVar14 = rsin(in_a0 * 0x44);
//     iVar14 = iVar14 * 0x7d;
//     if (iVar14 < 0) {
//       iVar14 = iVar14 + 0xfff;
//     }
//     uVar16 = (char)(iVar14 >> 0xc) + 0x80;
//     iVar14 = rsin(in_a0 * 0x44 + 0x200);
//     iVar14 = iVar14 * 0x7d;
//     if (iVar14 < 0) {
//       iVar14 = iVar14 + 0xfff;
//     }
//     PStack_48.r0 = (char)(iVar14 >> 0xc) + 0x80;
//     PStack_48.g0 = PStack_48.r0;
//     PStack_48.b0 = PStack_48.r0;
//     PStack_48.r1 = PStack_48.r0;
//     PStack_48.g1 = PStack_48.r0;
//     PStack_48.b1 = PStack_48.r0;
//     PStack_48.r2 = uVar16;
//     PStack_48.g2 = uVar16;
//     PStack_48.b2 = uVar16;
//     PStack_48.r3 = uVar16;
//     PStack_48.g3 = uVar16;
//     PStack_48.b3 = uVar16;
//     DrawPrim(&PStack_48);
//     PutDrawEnv(&local_108);
//   }
//   return;
// }
