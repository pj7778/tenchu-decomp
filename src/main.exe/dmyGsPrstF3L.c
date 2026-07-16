#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF3L (0x80066b94) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001511c[]; /* "PrstF3L\n" */
extern s32 D_8008f620;

void *dmyGsPrstF3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f620 == 0)
    {
        printf(D_8001511c);
        D_8008f620 = 1;
    }
    return arg3;
}
