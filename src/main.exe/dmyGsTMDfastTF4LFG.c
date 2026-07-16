#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTF4LFG (0x8006840c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800155a4[]; /* "TMDfastTF4LFG\n" */
extern s32 D_8008f77c;

void *dmyGsTMDfastTF4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f77c == 0)
    {
        printf(D_800155a4);
        D_8008f77c = 1;
    }
    return arg3;
}
