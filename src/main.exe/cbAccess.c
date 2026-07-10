#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/cbAccess", cbAccess);

// triage: EASY — 69 insns, 1 loop, 4 callees, ~0.11 to debug_menu_stage_option
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Type propagation algorithm not settling */
//
// void cbAccess(void)
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
//   uint uVar15;
//   DRAWENV *pDVar16;
//   DRAWENV *pDVar17;
//   undefined4 uVar18;
//   DISPENV local_e0;
//   DRAWENV local_c8;
//   DRAWENV local_68;
//
//   uVar15 = AccessPower + 8;
//   AccessPower = uVar15 & 0xff;
//   AccessImage.r0 = (u_char)uVar15;
//   AccessImage.b0 = 0xff - AccessImage.r0;
//   AccessImage.g0 = AccessImage.r0;
//   AccessImage.r1 = AccessImage.r0;
//   AccessImage.g1 = AccessImage.b0;
//   AccessImage.b1 = AccessImage.r0;
//   AccessImage.r2 = AccessImage.b0;
//   AccessImage.g2 = AccessImage.r0;
//   AccessImage.b2 = AccessImage.r0;
//   AccessImage.r3 = AccessImage.r0;
//   AccessImage.g3 = AccessImage.r0;
//   AccessImage.b3 = AccessImage.r0;
//   GetDrawEnv(&local_c8);
//   GetDispEnv(&local_e0);
//   pDVar13 = &local_c8;
//   pDVar14 = &local_68;
//   do {
//     pDVar17 = pDVar14;
//     pDVar16 = pDVar13;
//     sVar1 = (pDVar16->clip).y;
//     sVar2 = (pDVar16->clip).w;
//     sVar3 = (pDVar16->clip).h;
//     uVar18 = *(undefined4 *)pDVar16->ofs;
//     sVar4 = (pDVar16->tw).x;
//     sVar5 = (pDVar16->tw).y;
//     (pDVar17->clip).x = (pDVar16->clip).x;
//     (pDVar17->clip).y = sVar1;
//     (pDVar17->clip).w = sVar2;
//     (pDVar17->clip).h = sVar3;
//     *(undefined4 *)pDVar17->ofs = uVar18;
//     (pDVar17->tw).x = sVar4;
//     (pDVar17->tw).y = sVar5;
//     pDVar13 = (DRAWENV *)&(pDVar16->tw).w;
//     pDVar14 = (DRAWENV *)&(pDVar17->tw).w;
//   } while (pDVar13 != (DRAWENV *)(local_c8.dr_env.code + 0xc));
//   sVar1 = (pDVar16->tw).h;
//   uVar6 = pDVar16->tpage;
//   uVar7 = pDVar16->dtd;
//   uVar8 = pDVar16->dfe;
//   uVar9 = pDVar16->isbg;
//   uVar10 = pDVar16->r0;
//   uVar11 = pDVar16->g0;
//   uVar12 = pDVar16->b0;
//   (pDVar17->tw).w = (pDVar16->tw).w;
//   (pDVar17->tw).h = sVar1;
//   pDVar17->tpage = uVar6;
//   pDVar17->dtd = uVar7;
//   pDVar17->dfe = uVar8;
//   pDVar17->isbg = uVar9;
//   pDVar17->r0 = uVar10;
//   pDVar17->g0 = uVar11;
//   pDVar17->b0 = uVar12;
//   local_68.clip.x = local_e0.disp.x;
//   local_68.clip.y = local_e0.disp.y;
//   local_68.clip.w = local_e0.disp.w;
//   local_68.clip.h = local_e0.disp.h;
//   local_68.ofs[0] = local_e0.disp.x;
//   local_68.ofs[1] = local_e0.disp.y;
//   PutDrawEnv(&local_68);
//   DrawPrim(&AccessImage);
//   PutDrawEnv(&local_c8);
//   return;
// }
