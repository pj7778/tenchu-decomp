#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG3LFG (0x80067e6c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001547c[]; /* "TMDfastG3LFG\n" */
extern s32 D_8008f72c;

void *dmyGsTMDfastG3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f72c == 0)
    {
        printf(D_8001547c);
        D_8008f72c = 1;
    }
    return arg3;
}
