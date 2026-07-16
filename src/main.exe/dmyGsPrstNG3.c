#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstNG3 (0x80066e1c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001518c[]; /* "PrstNG3\n" */
extern s32 D_8008f644;

void *dmyGsPrstNG3(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f644 == 0)
    {
        printf(D_8001518c);
        D_8008f644 = 1;
    }
    return arg3;
}
