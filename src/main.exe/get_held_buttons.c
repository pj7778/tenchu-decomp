#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/get_held_buttons", get_held_buttons);

/*
buttons_held get_held_buttons(u32 param_1)

{
    FUN_8001ada4();
    return (&HELD_BUTTONS)[(param_1 & 3) * 7 + ((s32)param_1 >> 4) * 0x1c];
}
*/