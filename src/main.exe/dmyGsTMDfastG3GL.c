#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG3GL (0x800686dc) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015644[]; /* "TMDfastG3GL\n" */
extern s32 D_8008f7a4;

void *dmyGsTMDfastG3GL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7a4 == 0)
    {
        printf(D_80015644);
        D_8008f7a4 = 1;
    }
    return arg3;
}
