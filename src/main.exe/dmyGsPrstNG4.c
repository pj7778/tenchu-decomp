#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstNG4 (0x8006771c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015324[]; /* "PrstNG4\n" */
extern s32 D_8008f6c4;

void *dmyGsPrstNG4(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6c4 == 0)
    {
        printf(D_80015324);
        D_8008f6c4 = 1;
    }
    return arg3;
}
