#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetupSprite", SetupSprite);

// triage: EASY — 116 insns, 1 loop, 5 callees, ~0.41 to InitSprite
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// Sprite3D * SetupSprite(Sprite3D *orgsprt,GsIMAGE *image)
//
// {
//   short sVar1;
//   u_short uVar2;
//   Sprite3D *base;
//   Sprite3D *pSVar3;
//   Sprite3D *pSVar4;
//   Sprite3D *pSVar5;
//   Sprite3D *pSVar6;
//   undefined4 uVar7;
//   undefined4 uVar8;
//   long lVar9;
//   undefined4 uVar10;
//   uint tp;
//
//   base = (Sprite3D *)valloc(0x8c);
//   if (orgsprt == (Sprite3D *)0x0) {
//     GsInitCoordinate2(&World.locate,(GsCOORDINATE2 *)base);
//     (base->locate).coord.t[0] = 0;
//     (base->locate).coord.t[1] = 0;
//     (base->locate).coord.t[2] = 0;
//     (base->rotate).vx = 0;
//     (base->rotate).vy = 0;
//     (base->rotate).vz = 0;
//     (base->clip).vx = 0;
//     (base->clip).vy = 0;
//     (base->clip).vz = 0;
//     RotMatrixYXZ(&base->rotate,&(base->locate).coord);
//     (base->locate).flg = 0;
//     base->id = -1;
//     base->attribute = 0;
//     base->scale = 0x1000;
//     memset((uchar *)&base->sprite,'\0',0x24);
//     (base->sprite).attribute = 0;
//     (base->sprite).b = 0x80;
//     (base->sprite).g = 0x80;
//     (base->sprite).r = 0x80;
//     (base->sprite).scaley = 0x1000;
//     (base->sprite).scalex = 0x1000;
//     if (image != (GsIMAGE *)0x0) {
//       tp = (ushort)image->pmode & 3;
//       (base->sprite).attribute = (base->sprite).attribute | tp << 0x18;
//       (base->sprite).w = image->pw << (2 - tp & 0x1f);
//       (base->sprite).h = image->ph;
//       uVar2 = GetTPage(tp,0,(int)image->px,(int)image->py);
//       (base->sprite).tpage = uVar2;
//       (base->sprite).u =
//            (byte)((int)image->px << (2 - tp & 0x1f)) & (char)(1 << (8 - tp & 0x1f)) - 1U;
//       (base->sprite).v = (uchar)image->py;
//       (base->sprite).cx = image->cx;
//       sVar1 = image->cy;
//       (base->sprite).my = (base->sprite).h >> 1;
//       (base->sprite).mx = (base->sprite).w >> 1;
//       (base->sprite).cy = sVar1;
//     }
//   }
//   else {
//     pSVar4 = base;
//     pSVar6 = orgsprt;
//     do {
//       pSVar5 = pSVar6;
//       pSVar3 = pSVar4;
//       uVar7 = *(undefined4 *)(pSVar5->locate).coord.m[0];
//       uVar8 = *(undefined4 *)((pSVar5->locate).coord.m[0] + 2);
//       uVar10 = *(undefined4 *)((pSVar5->locate).coord.m[1] + 1);
//       (pSVar3->locate).flg = (pSVar5->locate).flg;
//       *(undefined4 *)(pSVar3->locate).coord.m[0] = uVar7;
//       *(undefined4 *)((pSVar3->locate).coord.m[0] + 2) = uVar8;
//       *(undefined4 *)((pSVar3->locate).coord.m[1] + 1) = uVar10;
//       pSVar6 = (Sprite3D *)((pSVar5->locate).coord.m + 2);
//       pSVar4 = (Sprite3D *)((pSVar3->locate).coord.m + 2);
//     } while (pSVar6 != (Sprite3D *)&(orgsprt->sprite).mx);
//     uVar7 = *(undefined4 *)((pSVar5->locate).coord.m[2] + 2);
//     lVar9 = (pSVar5->locate).coord.t[0];
//     (pSVar4->locate).flg = (pSVar6->locate).flg;
//     *(undefined4 *)((pSVar3->locate).coord.m[2] + 2) = uVar7;
//     (pSVar3->locate).coord.t[0] = lVar9;
//   }
//   return base;
// }
