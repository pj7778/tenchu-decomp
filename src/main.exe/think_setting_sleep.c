#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/think_setting_sleep", think_setting_sleep_);
/*
s32 think_setting_sleep_(void)
{

    s32 iVar1;
    u16 uVar2;

    uVar2 = 0;
    if (CHARACTER_BEING_UPDATED_->something_about_current_animation->animation_state_perhaps == 0x100)
    {
        ALERT_STATUS_ = -1;
    }
    else if (CHARACTER_BEING_UPDATED_->something_about_current_animation->frames_since_animation_start == 0)
    {
        uVar2 = 0x1001;
    }
    if ((FRAMES_UNTIL_END_OF_ALERT != 0) || (((s32)ACTUALLY_ALERT_STATUS_ & 0x8000U) != 0))
    {
        iVar1 = turn_towards_player_(0, 0);
        uVar2 = (u16)iVar1 & 0xa000;
    }
    return (s32)(s16)uVar2;
}
*/
