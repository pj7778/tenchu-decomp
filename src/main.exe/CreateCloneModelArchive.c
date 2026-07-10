#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * struct ModelArchiveType * CreateCloneModelArchive(struct ModelArchiveType *mad);
 *     3DCTRL.C:438, 27 src lines, frame 48 bytes, saved-reg mask 0x803f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s5       struct ModelArchiveType * mad
 *     reg   $s1       struct ModelArchiveType * newmad
 *     reg   $s4       short i
 *     reg   $s1       struct ModelType * dim
 *     reg   $s2       struct ModelType * objp
 *     reg   $s0       struct ModelType * objp
 *     reg   $s0       struct ModelType * dim
 *
 * Globals it touches, as the original declared them:
 *     extern struct ModelType World;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/CreateCloneModelArchive", CreateCloneModelArchive);

// triage: MEDIUM — 105 insns, 1 loop, 4 callees, ~0.25 to CreateCloneModel
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// ModelArchiveType * CreateCloneModelArchive(ModelArchiveType *mad)
//
// {
//   short sVar1;
//   ushort uVar2;
//   ModelArchiveType *base;
//   ModelType **ppMVar3;
//   GsCOORDINATE2 *base_00;
//   int iVar4;
//   int iVar5;
//   int iVar6;
//
//   if (mad == (ModelArchiveType *)0x0) {
//                     /* WARNING: Subroutine does not return */
//     SystemOut((uchar *)"NO SOURCE MODEL ARCHIVE DATA");
//   }
//   base = (ModelArchiveType *)valloc(0x6c);
//   iVar6 = 0;
//   uVar2 = mad->n;
//   base->n = uVar2;
//   ppMVar3 = (ModelType **)valloc((int)((uint)uVar2 << 0x10) >> 0xe);
//   base->object = ppMVar3;
//   GsInitCoordinate2((mad->locate).super,(GsCOORDINATE2 *)base);
//   (base->locate).coord.t[0] = 0;
//   (base->locate).coord.t[1] = 0;
//   (base->locate).coord.t[2] = 0;
//   (base->rotate).vx = 0;
//   (base->rotate).vy = 0;
//   (base->rotate).vz = 0;
//   (base->clip).vx = 0;
//   (base->clip).vy = 0;
//   (base->clip).vz = 0;
//   RotMatrixYXZ(&base->rotate,&(base->locate).coord);
//   sVar1 = base->n;
//   (base->locate).flg = 0;
//   base->id = -1;
//   base->attribute = 0;
//   if (0 < sVar1) {
//     iVar4 = 0;
//     do {
//       iVar5 = *(int *)((iVar4 >> 0xe) + (int)mad->object);
//       base_00 = (GsCOORDINATE2 *)valloc(0x74);
//       base_00[1].coord.t[0] = (long)base_00;
//       *(undefined4 *)(base_00[1].coord.m[2] + 2) = 0;
//       GsInitCoordinate2(&World.locate,base_00);
//       (base_00->coord).t[0] = 0;
//       (base_00->coord).t[1] = 0;
//       (base_00->coord).t[2] = 0;
//       *(undefined2 *)&base_00[1].flg = 0;
//       *(undefined2 *)((int)&base_00[1].flg + 2) = 0;
//       base_00[1].coord.m[0][0] = 0;
//       base_00[1].coord.m[1][1] = 0;
//       base_00[1].coord.m[1][2] = 0;
//       base_00[1].coord.m[2][0] = 0;
//       RotMatrixYXZ((SVECTOR *)(base_00 + 1),&base_00->coord);
//       base_00->flg = 0;
//       base_00[1].coord.m[0][2] = -1;
//       base_00[1].coord.m[1][0] = 0;
//       if (iVar5 != 0) {
//         base_00[1].coord.t[1] = *(long *)(iVar5 + 0x6c);
//       }
//       iVar6 = iVar6 + 1;
//       *(GsCOORDINATE2 **)((iVar4 >> 0xe) + (int)base->object) = base_00;
//       iVar4 = iVar6 * 0x10000;
//     } while (iVar6 * 0x10000 >> 0x10 < (int)base->n);
//   }
//   (base->rotate).pad = (short)((*base->object)->locate).coord.t[1];
//   return base;
// }
