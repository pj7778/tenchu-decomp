#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastF4NL (0x80068184) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015524[]; /* "TMDfastF4NL\n" */
extern s32 D_8008f758;

void *dmyGsTMDfastF4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f758 == 0)
    {
        printf(D_80015524);
        D_8008f758 = 1;
    }
    return arg3;
}
