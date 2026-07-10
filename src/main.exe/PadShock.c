#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void PadShock(int port, int p1, int p2);
 *     PADCMD.C:109, 12 src lines, frame 0 bytes, saved-reg mask 0x00000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       int port
 *     param $a1       int p1
 *     param $a2       int p2
 *
 * Globals it touches, as the original declared them:
 *     extern struct TPadPort PadPort[2][4];
 * END PSX.SYM */

/*
 * PadShock (0x8001b404) — set (or clear) the rumble-motor bytes at offset
 * 8/9 of PadPort[port>>4][port&3] (same row/col address arithmetic as
 * FUN_8001b4e0). D_8001005D (0x8001005d, one byte before the already-named
 * CHOSEN_LANGUAGE @0x8001005e in config/symbols) gates it: nonzero writes
 * the two motor bytes from p1/p2, zero clears both.
 */
extern u8 D_8001005D;

void PadShock(s32 port, s8 p1, s8 p2)
{
    u8 *p = (u8 *)&PadPort[port >> 4][port & 3];
    u8 *q = p;

    if (D_8001005D != 0) {
        p[8] = p1;
        p[9] = p2;
        return;
    }
    q[8] = 0;
    q[9] = 0;
}
