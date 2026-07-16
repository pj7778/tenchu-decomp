#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivG4LFG (0x800677ac) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001533c[]; /* "TMDdivG4LFG\n" */
extern s32 D_8008f6cc;

void *dmyGsTMDdivG4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6cc == 0)
    {
        printf(D_8001533c);
        D_8008f6cc = 1;
    }
    return arg3;
}
