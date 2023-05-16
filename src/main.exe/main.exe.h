typedef u16 buttons_held;

extern void FUN_8001ada4(void);
extern buttons_held HELD_BUTTONS;

typedef struct
{
    char *choice_name;
    u32 choice_number;
} debug_menu_choice;

// int debug_menu_choose(char *screen_header, debug_menu_choice *choices, char *param_3);