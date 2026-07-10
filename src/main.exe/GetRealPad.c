#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * long GetRealPad(int port);
 *     PADCMD.C:276, 5 src lines, frame 24 bytes, saved-reg mask 0x80010000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       int port
 *
 * Globals it touches, as the original declared them:
 *     extern struct TPadPort PadPort[2][4];
 * END PSX.SYM */

/*
 * Returns the held-buttons word for controller [arg0 >> 4][arg0 & 3].
 *
 * The pointer temporary forces GCC to compute the row index (`arg0 >> 4`) before
 * the column (`arg0 & 3`) — the order the original picks. Writing the natural
 * `return PadPort[arg0 >> 4][arg0 & 3].held;` computes them the other way and
 * doesn't byte-match. (With the old non-canonical cc1 this also needed a
 * `do/while(0)` wrapper; the canonical gcc-2.8.1-psx doesn't.)
 */
buttons_held GetRealPad(s32 port)
{
    buttons_held *held;
    PadProc();
    held = &PadPort[port >> 4][port & 3].held;
    return *held;
}
