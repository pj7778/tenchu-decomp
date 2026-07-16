#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTF3LFG (0x800670ec) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001520c[]; /* "TMDdivTF3LFG\n" */
extern s32 D_8008f66c;

void *dmyGsTMDdivTF3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f66c == 0)
    {
        printf(D_8001520c);
        D_8008f66c = 1;
    }
    return arg3;
}
