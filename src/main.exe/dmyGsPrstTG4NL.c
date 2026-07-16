#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTG4NL (0x80067ac4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800153cc[]; /* "PrstTG4NL\n" */
extern s32 D_8008f6f8;

void *dmyGsPrstTG4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6f8 == 0)
    {
        printf(D_800153cc);
        D_8008f6f8 = 1;
    }
    return arg3;
}
