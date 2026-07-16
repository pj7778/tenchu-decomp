#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG3GNL (0x8006876c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015664[]; /* "TMDfastG3GNL\n" */
extern s32 D_8008f7ac;

void *dmyGsTMDfastG3GNL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7ac == 0)
    {
        printf(D_80015664);
        D_8008f7ac = 1;
    }
    return arg3;
}
