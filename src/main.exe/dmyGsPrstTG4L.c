#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTG4L (0x80067b54) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800153e4[]; /* "PrstTG4L\n" */
extern s32 D_8008f700;

void *dmyGsPrstTG4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f700 == 0)
    {
        printf(D_800153e4);
        D_8008f700 = 1;
    }
    return arg3;
}
