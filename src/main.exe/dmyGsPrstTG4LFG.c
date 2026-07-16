#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTG4LFG (0x80067b0c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800153d8[]; /* "PrstTG4LFG\n" */
extern s32 D_8008f6fc;

void *dmyGsPrstTG4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6fc == 0)
    {
        printf(D_800153d8);
        D_8008f6fc = 1;
    }
    return arg3;
}
