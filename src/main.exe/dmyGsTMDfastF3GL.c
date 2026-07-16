#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastF3GL (0x80068604) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015614[]; /* "TMDfastF3GL\n" */
extern s32 D_8008f798;

void *dmyGsTMDfastF3GL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f798 == 0)
    {
        printf(D_80015614);
        D_8008f798 = 1;
    }
    return arg3;
}
