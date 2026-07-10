#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short DrawSprite(struct Sprite3D *sprt);
 *     3DCTRL.C:593, 14 src lines, frame 72 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s1       struct Sprite3D * sprt
 *     stack sp+16     struct MATRIX mat
 *     reg   $a2       long sz
 *     reg   $s1       struct ModelType * objp
 *     reg   $s2       long * xy
 *     reg   $v1       long sz
 *     reg   $s0       short atr
 *     stack sp+48     short [2] rxy
 *
 * Globals it touches, as the original declared them:
 *     extern struct SVECTOR UnitVector;
 *     extern struct GsOT *OTablePt;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawSprite", DrawSprite);

// triage: MEDIUM — 111 insns, mul/div, 4 callees, ~0.07 to GetAbsolutePosition
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
//
// short DrawSprite(Sprite3D *sprt)
//
// {
//   ushort uVar1;
//   GsOT *ot;
//   short sVar2;
//   long lVar3;
//   int iVar4;
//   int iVar5;
//   short *sxy;
//   MATRIX MStack_38;
//   short local_18;
//   short local_16;
//
//   GsGetLs(&sprt->locate,&MStack_38);
//   GsSetLsMatrix(&MStack_38);
//   uVar1 = sprt->attribute;
//   sxy = &(sprt->sprite).x;
//   if ((uVar1 & 1) == 0) {
//     if ((uVar1 & 2) == 0) {
//       lVar3 = RotTransPers(&sprt->clip,(long *)&local_18,(long *)0x0,(long *)0x0);
//       if (((uVar1 & 4) != 0) && (iVar4 = -1, lVar3 >> 2 == 0)) goto LAB_80017d14;
//       if ((uVar1 & 8) != 0) {
//         iVar5 = (int)local_18;
//         if (iVar5 < 0) {
//           iVar5 = -iVar5;
//         }
//         iVar4 = -1;
//         if (0xf0 < iVar5) goto LAB_80017d14;
//         iVar4 = (int)local_16;
//         if (iVar4 < 0) {
//           iVar4 = -iVar4;
//         }
//         if (0xb4 < iVar4) goto LAB_80017cdc;
//       }
//       if (((uVar1 & 0x10) != 0) && (iVar4 = -1, 0x4e2 < lVar3 >> 2)) goto LAB_80017d14;
//     }
//     lVar3 = RotTransPers(&UnitVector,(long *)sxy,(long *)0x0,(long *)0x0);
//     iVar4 = lVar3 >> 2;
//     if (iVar4 < 0x4e3) {
//       if (sxy == (short *)0x0) {
//         if (iVar4 < 300) {
//           _DrawTMDmode = 0;
//         }
//         else {
//           _DrawTMDmode = 0x20;
//         }
//       }
//       goto LAB_80017d14;
//     }
//   }
// LAB_80017cdc:
//   iVar4 = -1;
// LAB_80017d14:
//   ot = OTablePt;
//   iVar4 = iVar4 + -5;
//   if (iVar4 < 1) {
//     sVar2 = 0;
//   }
//   else {
//     iVar5 = (sprt->scale >> 2) * 300;
//     if (iVar4 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar4 == -1) && (iVar5 == -0x80000000)) {
//       trap(0x1800);
//     }
//     sVar2 = (short)(iVar5 / iVar4);
//     (sprt->sprite).scaley = sVar2;
//     (sprt->sprite).scalex = sVar2;
//     GsSortSprite(&sprt->sprite,ot,(ushort)iVar4);
//     sVar2 = 1;
//   }
//   return sVar2;
// }
