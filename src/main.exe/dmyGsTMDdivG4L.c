#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivG4L (0x800677f4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001534c[]; /* "TMDdivG4L\n" */
extern s32 D_8008f6d0;

void *dmyGsTMDdivG4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6d0 == 0)
    {
        printf(D_8001534c);
        D_8008f6d0 = 1;
    }
    return arg3;
}
