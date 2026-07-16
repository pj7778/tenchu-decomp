#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstG4NL (0x80067644) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015300[]; /* "PrstG4NL\n" */
extern s32 D_8008f6b8;

void *dmyGsPrstG4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6b8 == 0)
    {
        printf(D_80015300);
        D_8008f6b8 = 1;
    }
    return arg3;
}
