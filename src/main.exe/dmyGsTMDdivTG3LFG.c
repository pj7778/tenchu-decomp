#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTG3LFG (0x8006732c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015274[]; /* "TMDdivTG3LFG\n" */
extern s32 D_8008f68c;

void *dmyGsTMDdivTG3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f68c == 0)
    {
        printf(D_80015274);
        D_8008f68c = 1;
    }
    return arg3;
}
