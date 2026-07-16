#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivG4NL (0x80067764) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015330[]; /* "TMDdivG4NL\n" */
extern s32 D_8008f6c8;

void *dmyGsTMDdivG4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6c8 == 0)
    {
        printf(D_80015330);
        D_8008f6c8 = 1;
    }
    return arg3;
}
