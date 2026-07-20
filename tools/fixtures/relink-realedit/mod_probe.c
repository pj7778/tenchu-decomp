#include "common.h"

/* Real-edit runtime fixture: a brand-new translation unit whose function is
 * called from a grown existing game function (PadProc).  The counter proves
 * the new code executes every frame; the initialized magic proves new
 * initialized data is loaded at its linked address.  tools/pcsx_smoke.py
 * verifies both through --watch-counter/--watch-equals. */

u32 ModTickCount;
u32 ModMagic = 0x600DF00D;

void ModProbeTick(void)
{
    ModTickCount = ModTickCount + 1;
}
