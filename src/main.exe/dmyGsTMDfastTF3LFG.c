#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTF3LFG (0x80067f8c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800154b4[]; /* "TMDfastTF3LFG\n" */
extern s32 D_8008f73c;

void *dmyGsTMDfastTF3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f73c == 0)
    {
        printf(D_800154b4);
        D_8008f73c = 1;
    }
    return arg3;
}
