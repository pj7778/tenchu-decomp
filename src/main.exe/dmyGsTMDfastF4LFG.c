#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastF4LFG (0x800681cc) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015534[]; /* "TMDfastF4LFG\n" */
extern s32 D_8008f75c;

void *dmyGsTMDfastF4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f75c == 0)
    {
        printf(D_80015534);
        D_8008f75c = 1;
    }
    return arg3;
}
