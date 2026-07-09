#include "common.h"
#include "main.exe.h"

/*
 * SystemOut (0x80016e8c, 0x8 bytes) — same TU as vinit.c/vsize.c. The fatal
 * "system halt" hook: it never returns, and never even reads `string` (the
 * message is presumably only printed by a debug build of this function). The
 * whole body is a single self-jump, so the loop's head label coincides with
 * the function entry.
 */

void SystemOut(u8 *string)
{
    for (;;)
    {
    }
}
