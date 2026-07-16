#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTF3NL (0x800670a4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800151fc[]; /* "TMDdivTF3NL\n" */
extern s32 D_8008f668;

void *dmyGsTMDdivTF3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f668 == 0)
    {
        printf(D_800151fc);
        D_8008f668 = 1;
    }
    return arg3;
}
