#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastF3NL (0x80067d04) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015434[]; /* "TMDfastF3NL\n" */
extern s32 D_8008f718;

void *dmyGsTMDfastF3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f718 == 0)
    {
        printf(D_80015434);
        D_8008f718 = 1;
    }
    return arg3;
}
