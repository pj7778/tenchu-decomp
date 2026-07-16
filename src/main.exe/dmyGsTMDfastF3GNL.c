#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastF3GNL (0x80068694) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015634[]; /* "TMDfastF3GNL\n" */
extern s32 D_8008f7a0;

void *dmyGsTMDfastF3GNL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7a0 == 0)
    {
        printf(D_80015634);
        D_8008f7a0 = 1;
    }
    return arg3;
}
