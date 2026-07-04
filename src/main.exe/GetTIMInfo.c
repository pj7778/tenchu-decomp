#include "common.h"
#include "main.exe.h"

/* GetTIMInfo (0x80018ac8) — skips the leading u_long (TIM ID word) and hands
 * the rest of the buffer to the real PSY-Q GsGetTimInfo(); always "succeeds"
 * (returns 1). main.exe.h already declares this TU's prototype as
 * `void *GetTIMInfo(u_long *tim, GsIMAGE *im)` (both matched callers, AddMisc
 * and BriefingAndInventorySelectionScreen, discard the return value) — Ghidra
 * types the return `short`/literal 1, so return an explicit `(void *)1` to
 * stay compatible with the header's declared type. */

extern void GsGetTimInfo(u_long *tim, GsIMAGE *im);

void *GetTIMInfo(u_long *tim, GsIMAGE *im)
{
    GsGetTimInfo(tim + 1, im);
    return (void *)1;
}
