#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF3GLFG (0x800687fc) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015680[]; /* "PrstF3GLFG\n" */
extern s32 D_8008f7b4;

void *dmyGsPrstF3GLFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7b4 == 0)
    {
        printf(D_80015680);
        D_8008f7b4 = 1;
    }
    return arg3;
}
