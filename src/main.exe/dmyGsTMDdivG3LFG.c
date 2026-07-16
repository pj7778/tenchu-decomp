#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivG3LFG (0x80066eac) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800151a4[]; /* "TMDdivG3LFG\n" */
extern s32 D_8008f64c;

void *dmyGsTMDdivG3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f64c == 0)
    {
        printf(D_800151a4);
        D_8008f64c = 1;
    }
    return arg3;
}
