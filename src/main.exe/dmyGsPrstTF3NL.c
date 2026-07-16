#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTF3NL (0x80066f84) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800151cc[]; /* "PrstTF3NL\n" */
extern s32 D_8008f658;

void *dmyGsPrstTF3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f658 == 0)
    {
        printf(D_800151cc);
        D_8008f658 = 1;
    }
    return arg3;
}
