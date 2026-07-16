#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTNF3 (0x8006717c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015228[]; /* "TMDdivTNF3\n" */
extern s32 D_8008f674;

void *dmyGsTMDdivTNF3(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f674 == 0)
    {
        printf(D_80015228);
        D_8008f674 = 1;
    }
    return arg3;
}
