#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF4NL (0x80067404) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001529c[]; /* "PrstF4NL\n" */
extern s32 D_8008f698;

void *dmyGsPrstF4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f698 == 0)
    {
        printf(D_8001529c);
        D_8008f698 = 1;
    }
    return arg3;
}
