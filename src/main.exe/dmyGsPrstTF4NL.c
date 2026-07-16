#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTF4NL (0x80067884) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015364[]; /* "PrstTF4NL\n" */
extern s32 D_8008f6d8;

void *dmyGsPrstTF4NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6d8 == 0)
    {
        printf(D_80015364);
        D_8008f6d8 = 1;
    }
    return arg3;
}
