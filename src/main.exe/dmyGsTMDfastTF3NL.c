#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTF3NL (0x80067f44) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800154a4[]; /* "TMDfastTF3NL\n" */
extern s32 D_8008f738;

void *dmyGsTMDfastTF3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f738 == 0)
    {
        printf(D_800154a4);
        D_8008f738 = 1;
    }
    return arg3;
}
