#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTNG4 (0x80067b9c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800153f0[]; /* "PrstTNG4\n" */
extern s32 D_8008f704;

void *dmyGsPrstTNG4(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f704 == 0)
    {
        printf(D_800153f0);
        D_8008f704 = 1;
    }
    return arg3;
}
