#include "common.h"
#include "main.exe.h"

/*
 * lePackEnemyLayout (0x8003caa4, 0x48 bytes) — the save-side counterpart of
 * leRestoreEnemyLayout: if the caller's buffer is big enough (>= sizeof
 * (enemy), 0xFF0), pack (copy) the whole enemy-layout table into it;
 * otherwise report the shortfall via AdtMessageBox and skip the copy.
 * `le`=layout-enemy family (see leResetPath.c for TEnemyLayout, recovered
 * from the Ghidra type export).
 *
 * Matching notes (docs/matching-cookbook.md):
 *  - m2c undercounts BOTH calls' arg lists: a register carried in live from
 *    the caller and never overwritten before the call doesn't look like an
 *    "argument" to m2c's basic-block-local view. AdtMessageBox keeps `size`
 *    live in $a1 across the branch and memcpy keeps `buf` live in $a0 the
 *    same way — both need all 3 arguments from Ghidra's rendering, not
 *    m2c's 1-2 arg version.
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

extern TEnemyLayout enemy[0x1E];
extern void AdtMessageBox(char *fmt, ...);
extern void *memcpy(void *s1, void *s2, u32 n);
extern char D_80012174[]; /* "enemy storing size too small %d/%d" */

void lePackEnemyLayout(void *buf, u32 size)
{
    if (size < sizeof(enemy))
    {
        AdtMessageBox(D_80012174, size, sizeof(enemy));
    }
    else
    {
        memcpy(buf, enemy, sizeof(enemy));
    }
}
