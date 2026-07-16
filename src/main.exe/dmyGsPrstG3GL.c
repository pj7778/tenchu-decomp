#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstG3GL (0x8006888c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015698[]; /* "PrstG3GL\n" */
extern s32 D_8008f7bc;

void *dmyGsPrstG3GL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7bc == 0)
    {
        printf(D_80015698);
        D_8008f7bc = 1;
    }
    return arg3;
}
