#include "common.h"
#include "main.exe.h"

/*
 * GetPad (0x8001b144) — held-buttons for PadPort[arg0][0], the first slot of
 * port `arg0` (a `short`; unlike GetRealPad, which splits a combined
 * port*16+slot index via >>4/&3).
 *
 * STATUS: NON_MATCHING — a sign-extension shift-split with no natural-C form.
 * The target sign-extends `arg0` as sll16/sra12/sra4 (three shifts, the extend
 * fused with the PadPort element scale); a plain "index PadPort by a short"
 * ALWAYS compiles to the textbook sll16/sra16 (two shifts), and cc1's
 * fold/combine refolds every plain-arithmetic respelling back to it (nested
 * expression, two statements, K&R params, explicit pointer casts, a
 * static-inline helper, even a literal transcription of the asm shifts — all
 * verified against cc1 -da RTL dumps + the gcc-2.8.1 sources). The natural
 * draft below is the 2-shift form (a few bytes off).
 *
 * A BYTE-EXACT match exists, but only via a genuine optimizer barrier — an
 * empty register-constrained inline asm, which is NOVEL to this project (no
 * other matched function uses one, and this codebase has parked other sub-C
 * ties as NON_MATCHING rather than hack them):
 *
 *     s32 t = arg0 << 4;         // cc1 fuses extend + <<4 into one sra12
 *     __asm__("" : "+r"(t));     // blocks combine from refolding the >>4 below
 *     return PadPort[t >> 4][0].held;
 *
 * Parked pending a project decision on whether to accept codegen barriers.
 * GetPadXY (0x8001b480) shares this exact idiom twice (x and y).
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetPad", GetPad);
#else
s16 GetPad(short arg0)
{
    return PadPort[arg0][0].held;
}
#endif
