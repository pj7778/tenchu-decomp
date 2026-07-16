#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTF4LFG (0x800679ec) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800153a4[]; /* "TMDdivTF4LFG\n" */
extern s32 D_8008f6ec;

void *dmyGsTMDdivTF4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6ec == 0)
    {
        printf(D_800153a4);
        D_8008f6ec = 1;
    }
    return arg3;
}
