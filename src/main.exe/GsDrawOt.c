#include "common.h"
#include "main.exe.h"

void GsDrawOt(GsOT *ot)
{
    DrawOTag(ot->tag);
}

// m2c (mipsel-gcc-c reference — cleaner control flow + register
// temps straight from the asm; Ghidra above has the real types):
//
// ? DrawOTag(s32);                                    /* extern */
//
// void GsDrawOt(void *arg0) {
//     DrawOTag(arg0->unk10);
// }
