#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivF4LFG (0x8006756c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800152d8[]; /* "TMDdivF4LFG\n" */
extern s32 D_8008f6ac;

void *dmyGsTMDdivF4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6ac == 0)
    {
        printf(D_800152d8);
        D_8008f6ac = 1;
    }
    return arg3;
}
