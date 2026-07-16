#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG4NL (0x800682a4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001555c[]; /* "TMDfastG4NL\n" */
extern s32 D_8008f768;

void *dmyGsTMDfastG4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f768 == 0)
    {
        printf(D_8001555c);
        D_8008f768 = 1;
    }
    return arg3;
}
