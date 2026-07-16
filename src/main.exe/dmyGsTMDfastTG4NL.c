#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTG4NL (0x800684e4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800155d4[]; /* "TMDfastTG4NL\n" */
extern s32 D_8008f788;

void *dmyGsTMDfastTG4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f788 == 0)
    {
        printf(D_800155d4);
        D_8008f788 = 1;
    }
    return arg3;
}
