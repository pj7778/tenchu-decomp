#include "common.h"
#include "main.exe.h"

/*
 * SetLightning (0x80038f98, 0x44 bytes) — thin forwarder to SetLightningI,
 * inserting a constant `1` and re-widening its own narrow params.
 *
 * Matching notes (docs/matching-cookbook.md):
 *  - The item TU's own extern (ProcItemLightningBolt.c) declares this as
 *    `SetLightning(VECTOR *pos, VECTOR *target, s32 a, s32 b, s32 c)` — a
 *    DIFFERENT view than the defining TU's real signature below. Original
 *    TUs disagree with the defining TU's prototype; this file uses the
 *    real one, not the caller's.
 *  - Ghidra mistyped the 5th (stack-passed) param as `int b` (rendering the
 *    call as `(int)(short)b`) — it's really `short b`. All THREE trailing
 *    params (r/g register-passed, b stack-passed) get the IDENTICAL
 *    sll-16/sra-16 re-sign-extend, because cc1 mechanically re-widens ANY
 *    narrow parameter on its first use regardless of class — a genuine
 *    `int` stack parameter that's merely narrowed for the callee instead
 *    folds straight to one `lh` (combine sees "load then keep low 16 bits"
 *    as one instruction, since the parm is just a MEM and a same-sized
 *    local copy doesn't stop this — copy-propagation erases the copy
 *    first). The tell: a full `lw` + sll/sra on a stack parameter used
 *    exactly once, narrowed exactly once, with NO other use, means the
 *    param's OWN type is narrow (mechanical widen-on-use), not `int`
 *    truncated-for-the-call (which would compile shorter, as `lh`).
 */
extern void SetLightningI(VECTOR *start, VECTOR *end, s32 one, short r, short g, short b);

void SetLightning(VECTOR *start, VECTOR *end, short r, short g, short b)
{
    SetLightningI(start, end, 1, r, g, b);
}
