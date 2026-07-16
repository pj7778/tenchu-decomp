#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivF3L (0x80066cb4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015150[]; /* "TMDdivF3L\n" */
extern s32 D_8008f630;

void *dmyGsTMDdivF3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f630 == 0)
    {
        printf(D_80015150);
        D_8008f630 = 1;
    }
    return arg3;
}
