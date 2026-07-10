#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short DrawModel(struct ModelType *objp);
 *     3DCTRL.C:297, 11 src lines, frame 72 bytes, saved-reg mask 0x80030000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s1       struct ModelType * objp
 *     stack sp+16     struct MATRIX mat
 *     reg   $s1       struct ModelType * objp
 *     reg   $v1       long sz
 *     reg   $s0       short atr
 *     stack sp+48     short [2] rxy
 *
 * Globals it touches, as the original declared them:
 *     extern struct SVECTOR UnitVector;
 *     extern struct GsOT *OTablePt;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawModel", DrawModel);

// triage: EASY — 83 insns, 4 callees, ~0.10 to GetAbsolutePosition
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
//
// short DrawModel(ModelType *objp)
//
// {
//   ushort uVar1;
//   long lVar2;
//   int iVar3;
//   MATRIX MStack_38;
//   short local_18;
//   short local_16;
//
//   GsGetLs(&objp->locate,&MStack_38);
//   GsSetLsMatrix(&MStack_38);
//   uVar1 = objp->attribute;
//   iVar3 = -1;
//   if ((uVar1 & 1) != 0) goto LAB_80017360;
//   if ((uVar1 & 2) == 0) {
//     lVar2 = RotTransPers(&objp->clip,(long *)&local_18,(long *)0x0,(long *)0x0);
//     if (((uVar1 & 4) == 0) || (lVar2 >> 2 != 0)) {
//       if ((uVar1 & 8) == 0) {
// LAB_800172fc:
//         if (((uVar1 & 0x10) != 0) && (iVar3 = -1, 0x4e2 < lVar2 >> 2)) goto LAB_80017360;
//         goto LAB_8001730c;
//       }
//       iVar3 = (int)local_18;
//       if (iVar3 < 0) {
//         iVar3 = -iVar3;
//       }
//       if (iVar3 < 0xf1) {
//         iVar3 = (int)local_16;
//         if (iVar3 < 0) {
//           iVar3 = -iVar3;
//         }
//         if (iVar3 < 0xb5) goto LAB_800172fc;
//       }
//     }
//   }
//   else {
// LAB_8001730c:
//     lVar2 = RotTransPers(&UnitVector,(long *)0x0,(long *)0x0,(long *)0x0);
//     iVar3 = lVar2 >> 2;
//     if (iVar3 < 0x4e3) {
//       if (iVar3 < 300) {
//         _DrawTMDmode = 0;
//       }
//       else {
//         _DrawTMDmode = 0x20;
//       }
//       goto LAB_80017360;
//     }
//   }
//   iVar3 = -1;
// LAB_80017360:
//   if (iVar3 != -1) {
//     DrawTMD(&objp->object,OTablePt,0);
//   }
//   return (short)(iVar3 != -1);
// }
