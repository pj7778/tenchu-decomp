#include "common.h"
#include "main.exe.h"
#include "item.h"

/*
 * GetMotionID (0x8001c510, 0x74 bytes) — search mmp's registered-motion
 * table (MotionRegistType[], sentinel mid == -1) for the row whose `mid`
 * matches the requested id and return that row's `id`; on falling through to
 * the sentinel without a match, returns the sentinel row's own `id` instead.
 * Same search-with-break-then-read-index shape as GetAttackDBID.c (a near-
 * twin, which itself calls this to resolve the character's current motion
 * before its own BattleDB search) — just reading a struct field instead of
 * the loop index at the end.
 */

s16 GetMotionID(MotionManager *mmp, s16 mid)
{
    MotionRegistType *reg;
    s16 i;

    reg = mmp->motreg;
    i = 0;
    while (reg[i].mid != -1)
    {
        if (reg[i].mid == mid)
        {
            break;
        }
        i++;
    }
    return reg[i].id;
}
