#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_800190f4", FUN_800190f4);

// triage: MEDIUM — 81 insns, 1 loop, 9 callees, ~0.23 to initialise_font
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Type propagation algorithm not settling */
//
// void FUN_800190f4(void)
//
// {
//   undefined1 *puVar1;
//   uint uVar2;
//   uint *puVar3;
//   short sVar4;
//   short sVar5;
//   short sVar6;
//   short sVar7;
//   short sVar8;
//   u_short uVar9;
//   u_char uVar10;
//   u_char uVar11;
//   u_char uVar12;
//   u_char uVar13;
//   u_char uVar14;
//   u_char uVar15;
//   DRAWENV *pDVar16;
//   ulong *puVar17;
//   DRAWENV *pDVar18;
//   undefined4 uVar19;
//   DRAWENV *pDVar20;
//   DRAWENV *pDVar21;
//   GsIMAGE GStack_150;
//   undefined1 auStack_130 [40];
//   undefined1 auStack_108 [40];
//   DISPENV local_e0;
//   DRAWENV local_c8;
//   DRAWENV local_68;
//
//   puVar17 = FileRead("K:\\WORK\\CDIMAGE\\DEMO\\loading.tim");
//   GetTIMInfo(puVar17,&GStack_150);
//   LoadTIMAndFree(puVar17);
//   FUN_8004eaf0(&GStack_150,auStack_130,0xd4,0xde);
//   puVar17 = FileRead("K:\\WORK\\CDIMAGE\\DEMO\\load_ten.tim");
//   GetTIMInfo(puVar17,&GStack_150);
//   LoadTIMAndFree(puVar17);
//   FUN_8004eaf0(&GStack_150,auStack_108,0xd4,0xc0);
//   GetDrawEnv(&local_c8);
//   GetDispEnv(&local_e0);
//   pDVar16 = &local_68;
//   pDVar21 = &local_c8;
//   do {
//     pDVar20 = pDVar21;
//     pDVar18 = pDVar16;
//     sVar4 = (pDVar20->clip).y;
//     sVar5 = (pDVar20->clip).w;
//     sVar6 = (pDVar20->clip).h;
//     uVar19 = *(undefined4 *)pDVar20->ofs;
//     sVar7 = (pDVar20->tw).x;
//     sVar8 = (pDVar20->tw).y;
//     (pDVar18->clip).x = (pDVar20->clip).x;
//     (pDVar18->clip).y = sVar4;
//     (pDVar18->clip).w = sVar5;
//     (pDVar18->clip).h = sVar6;
//     *(undefined4 *)pDVar18->ofs = uVar19;
//     (pDVar18->tw).x = sVar7;
//     (pDVar18->tw).y = sVar8;
//     pDVar21 = (DRAWENV *)&(pDVar20->tw).w;
//     pDVar16 = (DRAWENV *)&(pDVar18->tw).w;
//   } while (pDVar21 != (DRAWENV *)(local_c8.dr_env.code + 0xc));
//   sVar4 = (pDVar20->tw).h;
//   uVar9 = pDVar20->tpage;
//   uVar10 = pDVar20->dtd;
//   uVar11 = pDVar20->dfe;
//   uVar12 = pDVar20->isbg;
//   uVar13 = pDVar20->r0;
//   uVar14 = pDVar20->g0;
//   uVar15 = pDVar20->b0;
//   (pDVar18->tw).w = (pDVar20->tw).w;
//   (pDVar18->tw).h = sVar4;
//   pDVar18->tpage = uVar9;
//   pDVar18->dtd = uVar10;
//   pDVar18->dfe = uVar11;
//   pDVar18->isbg = uVar12;
//   pDVar18->r0 = uVar13;
//   pDVar18->g0 = uVar14;
//   pDVar18->b0 = uVar15;
//   puVar1 = (undefined1 *)((int)&local_68.clip.y + 1);
//   uVar2 = (uint)puVar1 & 3;
//   puVar3 = (uint *)(puVar1 + -uVar2);
//   *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | (uint)local_e0.disp._0_4_ >> (3 - uVar2) * 8;
//   local_68.clip.x = local_e0.disp.x;
//   local_68.clip.y = local_e0.disp.y;
//   puVar1 = (undefined1 *)((int)&local_68.clip.h + 1);
//   uVar2 = (uint)puVar1 & 3;
//   puVar3 = (uint *)(puVar1 + -uVar2);
//   *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | (uint)local_e0.disp._4_4_ >> (3 - uVar2) * 8;
//   local_68.clip.w = local_e0.disp.w;
//   local_68.clip.h = local_e0.disp.h;
//   local_68.ofs[0] = local_e0.disp.x;
//   local_68.ofs[1] = local_e0.disp.y;
//   PutDrawEnv(&local_68);
//   DrawPrim(auStack_130);
//   DrawPrim(auStack_108);
//   DrawSync(0);
//   PutDrawEnv(&local_c8);
//   return;
// }
