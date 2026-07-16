#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG3NL (0x80067e24) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001546c[]; /* "TMDfastG3NL\n" */
extern s32 D_8008f728;

void *dmyGsTMDfastG3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f728 == 0)
    {
        printf(D_8001546c);
        D_8008f728 = 1;
    }
    return arg3;
}
