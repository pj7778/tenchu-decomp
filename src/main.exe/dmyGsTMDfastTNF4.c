#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTNF4 (0x8006849c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape), except
 * this "N" (no-light) variant takes only 3 register arguments: the asm
 * saves/returns $a2, not $a3 (verified: word 4 is `move s0,a2`, matching
 * every other byte of the family's 18-instruction shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800155c4[]; /* "TMDfastTNF4\n" */
extern s32 D_8008f784;

void *dmyGsTMDfastTNF4(void *arg0, void *arg1, void *arg2)
{
    if (D_8008f784 == 0)
    {
        printf(D_800155c4);
        D_8008f784 = 1;
    }
    return arg2;
}
