#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTNF3 (0x8006705c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800151f0[]; /* "PrstTNF3\n" */
extern s32 D_8008f664;

void *dmyGsPrstTNF3(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f664 == 0)
    {
        printf(D_800151f0);
        D_8008f664 = 1;
    }
    return arg3;
}
