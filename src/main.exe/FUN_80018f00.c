#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern int AccessPower;
 *     extern struct POLY_GT4 AccessImage;
 *     extern struct tag_TItem items[30];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80018f00", FUN_80018f00);

// triage: EASY — 68 insns, 1 loop, 5 callees, ~0.10 to debug_menu_stage_option
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Type propagation algorithm not settling */
//
// void FUN_80018f00(void)
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
//   u_char uVar12;
//   DRAWENV *pDVar13;
//   DRAWENV *pDVar14;
//   DRAWENV *pDVar15;
//   DRAWENV *pDVar16;
//   undefined4 uVar17;
//   DISPENV local_e0;
//   DRAWENV local_c8;
//   DRAWENV local_68;
//
//   VSyncCallback((f *)0x0);
//   if (-1 < AccessPower) {
//     AccessImage.r0 = '\0';
//     AccessImage.g0 = '\0';
//     AccessImage.b0 = '\0';
//     AccessImage.r1 = '\0';
//     AccessImage.g1 = '\0';
//     AccessImage.b1 = '\0';
//     AccessImage.r2 = '\0';
//     AccessImage.g2 = '\0';
//     AccessImage.b2 = '\0';
//     AccessImage.r3 = '\0';
//     AccessImage.g3 = '\0';
//     AccessImage.b3 = '\0';
//     GetDrawEnv(&local_c8);
//     GetDispEnv(&local_e0);
//     pDVar13 = &local_c8;
//     pDVar14 = &local_68;
//     do {
//       pDVar16 = pDVar14;
//       pDVar15 = pDVar13;
//       sVar1 = (pDVar15->clip).y;
//       sVar2 = (pDVar15->clip).w;
//       sVar3 = (pDVar15->clip).h;
//       uVar17 = *(undefined4 *)pDVar15->ofs;
//       sVar4 = (pDVar15->tw).x;
//       sVar5 = (pDVar15->tw).y;
//       (pDVar16->clip).x = (pDVar15->clip).x;
//       (pDVar16->clip).y = sVar1;
//       (pDVar16->clip).w = sVar2;
//       (pDVar16->clip).h = sVar3;
//       *(undefined4 *)pDVar16->ofs = uVar17;
//       (pDVar16->tw).x = sVar4;
//       (pDVar16->tw).y = sVar5;
//       pDVar13 = (DRAWENV *)&(pDVar15->tw).w;
//       pDVar14 = (DRAWENV *)&(pDVar16->tw).w;
//     } while (pDVar13 != (DRAWENV *)(local_c8.dr_env.code + 0xc));
//     sVar1 = (pDVar15->tw).h;
//     uVar6 = pDVar15->tpage;
//     uVar7 = pDVar15->dtd;
//     uVar8 = pDVar15->dfe;
//     uVar9 = pDVar15->isbg;
//     uVar10 = pDVar15->r0;
//     uVar11 = pDVar15->g0;
//     uVar12 = pDVar15->b0;
//     (pDVar16->tw).w = (pDVar15->tw).w;
//     (pDVar16->tw).h = sVar1;
//     pDVar16->tpage = uVar6;
//     pDVar16->dtd = uVar7;
//     pDVar16->dfe = uVar8;
//     pDVar16->isbg = uVar9;
//     pDVar16->r0 = uVar10;
//     pDVar16->g0 = uVar11;
//     pDVar16->b0 = uVar12;
//     local_68.clip.x = local_e0.disp.x;
//     local_68.clip.y = local_e0.disp.y;
//     local_68.clip.w = local_e0.disp.w;
//     local_68.clip.h = local_e0.disp.h;
//     local_68.ofs[0] = local_e0.disp.x;
//     local_68.ofs[1] = local_e0.disp.y;
//     PutDrawEnv(&local_68);
//     DrawPrim(&AccessImage);
//     PutDrawEnv(&local_c8);
//   }
//   return;
// }
