#include "common.h"
#include "main.exe.h"

/*
 * Maintains one controller's current/previous/new input words and the recent
 * non-zero input history. `frames_since_new_input` is signed deliberately:
 * the increment-and-negative test is the original input-disable gate.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/update_pressed_buttons", update_pressed_buttons);
#else
s16 update_pressed_buttons(some_character_button_values *buttons, u16 pressed)
{
    s16 i;
    u16 previously_pressed;

    if (buttons->frames_since_new_input < 6 && ++buttons->frames_since_new_input < 0) {
        pressed = 0;
    }

    previously_pressed = buttons->currently_pressed;
    buttons->currently_pressed = pressed;
    buttons->pressed_last_frame = previously_pressed;
    buttons->newly_pressed = pressed & ~previously_pressed;

    if (buttons->pressed_last_frame != pressed && pressed != 0) {
        if (buttons->frames_since_new_input > 5) {
            buttons->buttons_pressed_in_s16_succession[0] = 0;
        }

        for (i = 3; i > 0; i--) {
            buttons->buttons_pressed_in_s16_succession[i] =
                buttons->buttons_pressed_in_s16_succession[i - 1];
        }

        buttons->frames_since_new_input = 0;
        buttons->buttons_pressed_in_s16_succession[0] = buttons->currently_pressed;
    }

    return pressed;
}
#endif
