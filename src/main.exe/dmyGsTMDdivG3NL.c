#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivG3NL (0x80066e64) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015198[]; /* "TMDdivG3NL\n" */
extern s32 D_8008f648;

void *dmyGsTMDdivG3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f648 == 0)
    {
        printf(D_80015198);
        D_8008f648 = 1;
    }
    return arg3;
}
