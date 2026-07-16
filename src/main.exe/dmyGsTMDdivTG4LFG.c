#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTG4LFG (0x80067c2c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001540c[]; /* "TMDdivTG4LFG\n" */
extern s32 D_8008f70c;

void *dmyGsTMDdivTG4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f70c == 0)
    {
        printf(D_8001540c);
        D_8008f70c = 1;
    }
    return arg3;
}
