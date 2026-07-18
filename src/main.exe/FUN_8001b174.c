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
u8 FUN_8001b174(short arg0)
{
    return ((u8 *)&PadPort[arg0][0])[6];
}
#endif
