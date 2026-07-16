#include "common.h"
#include "main.exe.h"
#include "gte.h"

/*
 * SetDepthQ (0x8005d01c, 0x10 bytes) — loads the GTE depth-cue control
 * registers: DQA (cr27) and DQB (cr28). First function matched under the
 * restricted gte.h inline-asm policy (docs/gte-policy.md).
 */

void SetDepthQ(s32 dqa, s32 dqb)
{
    gte_ldDQA(dqa);
    gte_ldDQB(dqb);
}
