#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern struct TPadPort PadPort[2][4];
 * END PSX.SYM */

/*
 * FUN_8001b174 (0x8001b174) — reads byte offset 6 of PadPort[arg0][0] (the low
 * byte of controller_input.unk_2[2]); GetPad's twin, same index math and the
 * same sign-extension shift-split.
 *
 * STATUS: NON_MATCHING — same sll16/sra12/sra4 shift-split as GetPad; the
 * natural 2-shift draft below is a few bytes off. Byte-exact only via GetPad's
 * inline-asm barrier idiom
 * (`s32 t = arg0 << 4; __asm__("" : "+r"(t)); … &PadPort[t >> 4][0] …`), parked
 * pending the same project decision on codegen barriers. See GetPad.c.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8001b174", FUN_8001b174);
#else
// DO NOT EDIT THIS
// add your version beneath it
//
// u8 FUN_8001b174(short arg0)
// {
//     return ((u8 *)&PadPort[arg0][0])[6];
// }
//
//
/* UPDATE: MATCHED asm-free (matchdiff: 0 differing bytes) — supersedes the
 * STATUS block above; no inline-asm barrier is needed. Three mechanisms, all
 * from GetPad.c:
 *  - `no << 4` + the `pad & 3` second use of `pad` keeps combine from
 *    refolding `(no<<4)>>4` back to the 2-shift sll16/sra16; the fused
 *    sll16/sra12/sra4 extend survives. `pad & 3` folds to 0 (column 0), so it
 *    costs no instruction — it exists only to keep `pad` multiply-used.
 *  - the pointer temp `p` materialises the base once, AFTER the ×56 offset, so
 *    lui/addiu land late (a direct `PadPort[..][..]` expr hoists them early,
 *    or folds `+6` into `%hi(PadPort+6)` — both a few bytes off).
 *  - `p[6]` emits `lbu 0x6(p)`; offset 6 is fAnalog / unk_2[2] low byte. */
u8 FUN_8001b174(short no)
{
    u8 *p;
    s32 pad = no << 4;
    p = (u8 *)&PadPort[pad >> 4][pad & 3];
    return p[6];
}
#endif
