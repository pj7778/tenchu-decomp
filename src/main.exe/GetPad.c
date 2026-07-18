#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short GetPad(short no);
 *     PADCMD.C:293, 15 src lines, frame 8 bytes, saved-reg mask 0x00000000 (DEMO build -- see below)
 *
 * Original parameters and locals (the demo COUNT and TYPES are high-value
 * codegen evidence, not a retail spec: an earlier-build helper/API change
 * can replace either). Retail access widths and callee ABI win. A repeated
 * name is a nested-block scope, not a duplicate.
 * A ZERO-locals record is unverified, not a claim that the function has none:
 * vfree lists zero locals yet its byte-matched source needs seven.
 * The frame size and saved-reg mask above are the DEMO's: retail often needs
 * FEWER callee-saved registers (measured: Think1random exact; Think1chase's
 * 0x800f0000 = s0-s3+ra vs retail's s0,s1,ra). Treat them as an upper bound
 * and a hint at how many values stay live, never as a spec. The asm wins.
 * Locals:
 *     param $a0       short no
 *
 * Globals it touches, as the original declared them:
 *     extern struct TPadPort PadPort[2][4];
 * END PSX.SYM */

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
 *
 * --- UPDATE (later verification against the real build + the full cc1 zoo) ---
 * Two caveats to the above, both from running it rather than reasoning:
 *  - The "BYTE-EXACT via the barrier" isn't end-to-end. The barrier fixes the
 *    fold (sll16/sra12/sra4 emit correctly), but a SECOND sub-C tie remains — an
 *    address-materialisation SCHEDULE. The target computes the x56 element offset
 *    first and materialises `lui/addiu PadPort` LATE; the barrier build hoists it
 *    EARLY, so it lands 20 bytes off (verified). A combine barrier can't reach a
 *    scheduler decision, so the barrier alone does not match.
 *  - Compiler version doesn't help either tie. Swept the un-barriered
 *    `(no<<4)>>4` idiom through all 8 PSX GCC versions (2.7.0..2.8.1, tools/gcc):
 *    every one refolds to sll16/sra16, and none schedules `PadPort` late.
 * Net: two ties (fold + schedule); a full match would need two stacked barriers,
 * against the "park, don't hack" norm. Still NON_MATCHING. (GetPadXY the same.)
 */

/* --- UPDATE 2: MATCHED, asm-free — the analysis above was right about the
 * fold and wrong about everything else. The original source is (almost
 * certainly) GetRealPad's own matched body with the port pre-shifted:
 * GetPad(no) == GetRealPad-minus-PadProc(no << 4). Three mechanisms, all
 * verified against cc1-281 with the real build flags:
 *  - The SECOND use of `pad` (the `& 3` column index) is what preserves the
 *    3-shift extend: combine cannot delete `pad = no << 4` while `pad` is
 *    still used elsewhere, so the sll16/sra12 + sra4 pair survives — no
 *    barrier needed, multi-use IS the barrier.
 *  - The `& 3` chain itself costs nothing: `(no << 4) & 3` is provably 0
 *    (low 4 bits clear), and the whole column-offset computation folds away.
 *  - The "late lui/addiu" is not a scheduler tie — nothing in this build is
 *    scheduled; order is pure expansion order, and GetRealPad's
 *    pointer-temporary shape (`held = &...; return *held`) expands the base
 *    address last, exactly where the target has it.
 * (`.held` is u16 in our headers but the target loads `lh`, hence the s16
 * cast; the original's field was presumably signed.)
 */

/* --- UPDATE 3: the cast drops out once `buttons_held` is s16 (game_types.h).
 * With the field signed, `.held` is already a signed lvalue, so the read
 * emits `lh` on its own — the `(s16 *)` in UPDATE 2 is redundant. Flipping
 * u16->s16 "made no difference" only because the cast was still forcing `lh`
 * regardless. Drop the cast and the matched source is a verbatim mirror of
 * GetRealPad (minus PadProc, port pre-shifted by `no << 4`):
 *     buttons_held GetPad(short no) {
 *         buttons_held *held;
 *         s32 pad = no << 4;
 *         held = &PadPort[pad >> 4][pad & 3].held;
 *         return *held;
 *     }
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetPad", GetPad);
#else
// s16 GetPad(short no)
// {
//     s16 *held;
//     s32 pad = no << 4;
//     held = (s16 *)&PadPort[pad >> 4][pad & 3].held;
//     return *held;
// }

// non-ugly and non-matching alternative
//
// s16 GetPad(short no)
// {
//     return PadPort[no][0].held;
// }

// matched: .held is s16, read directly -> lh (no cast); pointer temp defers
// the base address; the & 3 second use of pad preserves the 3-shift extend.
s16 GetPad(short no)
{
    s16 *held;
    s32 pad = no << 4;
    held = &PadPort[pad >> 4][pad & 3].held;
    return *held;
}
#endif
