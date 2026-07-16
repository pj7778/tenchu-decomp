#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTG3L (0x800680f4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015504[]; /* "TMDfastTG3L\n" */
extern s32 D_8008f750;

void *dmyGsTMDfastTG3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f750 == 0)
    {
        printf(D_80015504);
        D_8008f750 = 1;
    }
    return arg3;
}
