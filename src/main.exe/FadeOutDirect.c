#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FadeOutDirect", FadeOutDirect);

// triage: MEDIUM — 104 insns, 1 loop, 6 callees, ~0.11 to leAddPath
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Type propagation algorithm not settling */
//
// void FadeOutDirect(short time,short attrib,uchar r,uchar g)
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
//   undefined4 uVar15;
//   DRAWENV *pDVar16;
//   DRAWENV *pDVar17;
//   DISPENV local_118;
//   DRAWENV local_100;
//   DRAWENV local_a0;
//   undefined1 auStack_40 [3];
//   undefined1 local_3d;
//   uint local_3c;
//   undefined1 auStack_38 [3];
//   undefined1 local_35;
//   uchar local_34;
//   uchar local_33;
//   undefined1 local_31;
//   undefined2 local_30;
//   undefined2 local_2e;
//   short local_2c;
//   undefined2 local_2a;
//   undefined2 local_28;
//   short local_26;
//   short local_24;
//   short local_22;
//
//   GetDrawEnv(&local_100);
//   GetDispEnv(&local_118);
//   pDVar13 = &local_a0;
//   pDVar17 = &local_100;
//   do {
//     pDVar16 = pDVar17;
//     pDVar14 = pDVar13;
//     sVar1 = (pDVar16->clip).y;
//     sVar2 = (pDVar16->clip).w;
//     sVar3 = (pDVar16->clip).h;
//     uVar15 = *(undefined4 *)pDVar16->ofs;
//     sVar4 = (pDVar16->tw).x;
//     sVar5 = (pDVar16->tw).y;
//     (pDVar14->clip).x = (pDVar16->clip).x;
//     (pDVar14->clip).y = sVar1;
//     (pDVar14->clip).w = sVar2;
//     (pDVar14->clip).h = sVar3;
//     *(undefined4 *)pDVar14->ofs = uVar15;
//     (pDVar14->tw).x = sVar4;
//     (pDVar14->tw).y = sVar5;
//     pDVar17 = (DRAWENV *)&(pDVar16->tw).w;
//     pDVar13 = (DRAWENV *)&(pDVar14->tw).w;
//   } while (pDVar17 != (DRAWENV *)(local_100.dr_env.code + 0xc));
//   sVar1 = (pDVar16->tw).h;
//   uVar6 = pDVar16->tpage;
//   uVar7 = pDVar16->dtd;
//   uVar8 = pDVar16->dfe;
//   uVar9 = pDVar16->isbg;
//   uVar10 = pDVar16->r0;
//   uVar11 = pDVar16->g0;
//   uVar12 = pDVar16->b0;
//   (pDVar14->tw).w = (pDVar16->tw).w;
//   (pDVar14->tw).h = sVar1;
//   pDVar14->tpage = uVar6;
//   pDVar14->dtd = uVar7;
//   pDVar14->dfe = uVar8;
//   pDVar14->isbg = uVar9;
//   pDVar14->r0 = uVar10;
//   pDVar14->g0 = uVar11;
//   pDVar14->b0 = uVar12;
//   local_a0.clip.x = local_118.disp.x;
//   local_a0.clip.y = local_118.disp.y;
//   local_a0.clip.w = local_118.disp.w;
//   local_a0.clip.h = local_118.disp.h;
//   local_a0.ofs[0] = local_118.disp.x;
//   local_a0.ofs[1] = local_118.disp.y;
//   PutDrawEnv(&local_a0);
//   local_35 = 5;
//   local_31 = 0x2a;
//   local_3d = 1;
//                     /* Probable PsyQ macro: setDrawTPage() if setlen(p, 1), setDrawMode() if
//                        setlen(p, 2). */
//   local_3c = ((ushort)attrib & 3) << 5 | 0xe1000200;
//   local_30 = 0;
//   local_2e = 0;
//   local_2a = 0;
//   local_28 = 0;
//   local_2c = local_118.disp.w;
//   local_26 = local_118.disp.h;
//   local_24 = local_118.disp.w;
//   local_22 = local_118.disp.h;
//   local_34 = r;
//   local_33 = g;
//   for (; time != 0; time = time + -1) {
//     DrawPrim(auStack_38);
//     DrawPrim(auStack_40);
//     DrawSync(0);
//     VSync(0);
//   }
//   PutDrawEnv(&local_100);
//   return;
// }
