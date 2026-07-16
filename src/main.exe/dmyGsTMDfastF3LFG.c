#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastF3LFG (0x80067d4c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015444[]; /* "TMDfastF3LFG\n" */
extern s32 D_8008f71c;

void *dmyGsTMDfastF3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f71c == 0)
    {
        printf(D_80015444);
        D_8008f71c = 1;
    }
    return arg3;
}
