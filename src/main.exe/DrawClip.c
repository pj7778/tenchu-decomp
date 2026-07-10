#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawClip", DrawClip);

// triage: EASY — 74 insns, 1 callees, ~0.05 to DoBriefingAndInventorySelection
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
//
// long DrawClip(ModelType *objp,long *xy)
//
// {
//   ushort uVar1;
//   long lVar2;
//   int iVar3;
//   short local_18;
//   short local_16;
//
//   uVar1 = objp->attribute;
//   if ((uVar1 & 1) == 0) {
//     if ((uVar1 & 2) == 0) {
//       lVar2 = RotTransPers(&objp->clip,(long *)&local_18,(long *)0x0,(long *)0x0);
//       if (((uVar1 & 4) != 0) && (lVar2 >> 2 == 0)) {
//         return -1;
//       }
//       if ((uVar1 & 8) != 0) {
//         iVar3 = (int)local_18;
//         if (iVar3 < 0) {
//           iVar3 = -iVar3;
//         }
//         if (0xf0 < iVar3) {
//           return -1;
//         }
//         iVar3 = (int)local_16;
//         if (iVar3 < 0) {
//           iVar3 = -iVar3;
//         }
//         if (0xb4 < iVar3) {
//           return -1;
//         }
//       }
//       if (((uVar1 & 0x10) != 0) && (0x4e2 < lVar2 >> 2)) {
//         return -1;
//       }
//     }
//     lVar2 = RotTransPers(&UnitVector,xy,(long *)0x0,(long *)0x0);
//     iVar3 = lVar2 >> 2;
//     if (iVar3 < 0x4e3) {
//       if (xy != (long *)0x0) {
//         return iVar3;
//       }
//       if (iVar3 < 300) {
//         _DrawTMDmode = 0;
//         return iVar3;
//       }
//       _DrawTMDmode = 0x20;
//       return iVar3;
//     }
//   }
//   return -1;
// }
