#include "common.h"
#include "main.exe.h"

s16 Think1sleep(void)
{
    something_about_current_animation *temp_a0;
    u16 uVar1;

    temp_a0 = Me_THINK_C->something_about_current_animation;
    uVar1 = 0;
    if (temp_a0->animation_state_perhaps == 0x100)
    {
        SR = -1;
    }
    else if (temp_a0->frames_since_animation_start == 0)
    {
        uVar1 = 0x1001;
    }
    if ((FRAMES_UNTIL_END_OF_ALERT != 0) || ((Attrib & 0x8000) != 0))
    {
        uVar1 = turn_towards_player_(0, 0);
        uVar1 = uVar1 & 0xA000;
    }
    return uVar1;
}
