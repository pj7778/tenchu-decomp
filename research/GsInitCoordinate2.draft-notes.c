#include "common.h"
#include "main.exe.h"

/*
 * GsInitCoordinate2 (0x80065064) — stock PsyQ libgs: initialise a coordinate
 * system. Reset `base`'s matrix to the identity, hang it under `super`, and — if
 * `super` is a real node rather than the WORLD sentinel — link it back as
 * super's sub.
 *
 * `super > (GsCOORDINATE2 *)1` is PsyQ's WORLD test; as a pointer compare it is
 * UNSIGNED, which is the `sltiu a0,a0,2` (skip when super < 2). The write-back
 * reads `base->super` rather than reusing the parameter — the target reloads it
 * (`lw v0,72(a3)`), so the source really does say `base->super->sub`.
 *
 * The identity reset is a whole-MATRIX struct assignment; cc1 expands it to the
 * word-by-word copy out of GsIDMATRIX.
 */
extern MATRIX GsIDMATRIX;

/*
 * STATUS: NON_MATCHING IN THE REPOSITORY PROFILE, but the C below is exact under
 * Sony's real PsyQ 4.3 CC1PSX.EXE (`2.7.2.SN32.3.7 Build 0001`). Its 28
 * instructions, relocations, and linked bytes all reproduce the target. The
 * default PsyQ 4.4/4.5 2.8.1 compiler still emits only 27 instructions:
 *
 *   TARGET                        OURS
 *     move  a3,a1                 (absent)
 *     lui   a2,0x800c             lui   v0,0x800c
 *     addiu a2,a2,25856           addiu t0,v0,25856
 *     ... scratch v0,v1,a1        ... scratch v1,a2,a3
 *     ... base in a3              ... base in a1
 *
 * THE TELL: the target materialises &GsIDMATRIX into ONE register in place
 * (`lui a2 / addiu a2,a2`) and then uses a1 as a COPY SCRATCH — which is why it
 * must first move base out of a1 into a3, and that move is the whole length
 * difference. We split the address across TWO registers (`lui v0 / addiu t0,v0`)
 * and reach for t0. mips.h defines no REG_ALLOC_ORDER so find_reg walks
 * numerically (global.c:960, local-alloc.c:2266) — the target uses NO t-register
 * at all (a0..a3, v0, v1), so our t0 says we hold one more value live than it
 * does, at the point where the address is formed.
 *
 * This does NOT justify selecting 2.7.2 for this function alone. The original
 * PsyQ 4.5 MATRIX.OBJ contains ten public functions, in this order:
 *
 *   GsInitCoordinate2, GsInitCoord2param, GsSetLsMatrix, GsSetLightMatrix,
 *   GsSetLightMatrix2, GsMulCoord0, GsMulCoord2, GsMulCoord3, print_matrix,
 *   print_vector
 *
 * (DSTACK is the same member's 128-byte BSS object; GsSetNearClip is the next
 * archive member, GS_101.OBJ.) Compiler selection must therefore be made for
 * all ten functions as one object.
 *
 * The whole-object check rejects the packaged 4.3 compiler: unchanged, already
 * exact C for GsSetLsMatrix and GsMulCoord0/2/3 reproduces every target
 * instruction except the non-leaf return. Build 0001 emits
 *
 *     addiu sp,sp,N; jr ra; nop
 *
 * where the target and 2.8.1 emit
 *
 *     jr ra; addiu sp,sp,N
 *
 * Explicit delayed-branch/scheduler flags, optimisation levels, and natural
 * explicit-return spellings do not change that 2.7.2 epilogue. Applying a
 * compiler override just to GsInitCoordinate2 would hide this contradiction.
 *
 * Primary archive evidence explains why a tempting single-version attribution
 * fails. PsyQ 4.3 LIBGS stores the routines as separate MATRIX1..11.OBJ members:
 * MATRIX2's GsInitCoordinate2 core is the exact 2.7.2 shape above, while
 * MATRIX4/7/8/9 have the target's 2.8.1-style filled return slots. After
 * removing each member's section-end alignment, all ten 4.3 text cores are
 * byte-identical to the corresponding slices of the combined PsyQ 4.4/4.5
 * MATRIX.OBJ; the 4.4 and 4.5 objects are themselves byte-identical. The
 * library member therefore preserves mixed historical code-generation
 * provenance (or an unavailable internal compiler build), despite being one
 * linker object in the game. Until one executable reproduces every carved
 * member, keep the normal whole-object profile and this correct source guarded.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GsInitCoordinate2", GsInitCoordinate2);
#else
void GsInitCoordinate2(GsCOORDINATE2 *super, GsCOORDINATE2 *base)
{
    base->coord = GsIDMATRIX;
    base->super = super;
    base->flg = 0;
    if (super > (GsCOORDINATE2 *)1) {
        base->super->sub = base;
    }
}
#endif
