#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG3L (0x80067eb4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001548c[]; /* "TMDfastG3L\n" */
extern s32 D_8008f730;

void *dmyGsTMDfastG3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f730 == 0)
    {
        printf(D_8001548c);
        D_8008f730 = 1;
    }
    return arg3;
}
