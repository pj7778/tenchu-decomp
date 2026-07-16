#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTF3L (0x80067134) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001521c[]; /* "TMDdivTF3L\n" */
extern s32 D_8008f670;

void *dmyGsTMDdivTF3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f670 == 0)
    {
        printf(D_8001521c);
        D_8008f670 = 1;
    }
    return arg3;
}
