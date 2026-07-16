#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivF4NL (0x80067524) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800152cc[]; /* "TMDdivF4NL\n" */
extern s32 D_8008f6a8;

void *dmyGsTMDdivF4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6a8 == 0)
    {
        printf(D_800152cc);
        D_8008f6a8 = 1;
    }
    return arg3;
}
