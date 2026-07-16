#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTG3NL (0x80068064) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800154e4[]; /* "TMDfastTG3NL\n" */
extern s32 D_8008f748;

void *dmyGsTMDfastTG3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f748 == 0)
    {
        printf(D_800154e4);
        D_8008f748 = 1;
    }
    return arg3;
}
