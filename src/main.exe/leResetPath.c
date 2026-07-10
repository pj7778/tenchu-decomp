#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void leResetPath(int id);
 *     WORLD.C:1293, 6 src lines, frame 0 bytes, saved-reg mask 0x00000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       int id
 *
 * Globals it touches, as the original declared them:
 *     extern struct TEnemyLayout enemy[30];
 * END PSX.SYM */

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
