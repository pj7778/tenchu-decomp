#include "common.h"
#include "main.exe.h"

/*
 * leResetPath (0x8003ca4c, 0x2c bytes) — clears the path point-count on one
 * slot of the enemy-layout table (debug menu "path layout > reset path").
 *
 * enemy is TEnemyLayout[0x1e] (0x88/entry: type/ThinkType/nPath, x/y/z,
 * r/pad, VECTOR path[7] — full layout from the Ghidra type export, size
 * matches the asm's id*0x88 stride exactly). Only nPath (s16 @ offset 4) is
 * touched here.
 */

typedef struct
{
    s16 type;
    s16 ThinkType;
    s16 nPath;
    s32 x;
    s32 y;
    s32 z;
    s16 r;
    s16 pad;
    VECTOR path[7];
} TEnemyLayout; /* 0x88 */

extern TEnemyLayout enemy[];

void leResetPath(s32 id)
{
    if ((u32)id < 0x1E)
    {
        enemy[id].nPath = 0;
    }
}
