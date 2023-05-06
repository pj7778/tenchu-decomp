extern char D_80089f00;
extern char D_800976e4;
extern char D_80097b88;
extern char D_80097b8c;
extern char *s_shuriken;
extern void *PTR_s_shuriken;
extern char *s_JAPANESE;      // = "JAPANESE";
extern char **PTR_s_JAPANESE; // = &s_JAPANESE;
extern char UNK_800124b8;
extern char *s_AYAME; // = "AYAME";
extern char D_80011f18;
extern char *s__2d__s;
extern char D_80097dc8;
extern char *D_8001420c;     // = "language select";
extern char *D_8001421c;     // = "player select";
extern char *D_8001422c;     // = "stage select";
extern char *s_RIKIMARU;     // = "RIKIMARU";
extern char *PTR_s_RIKIMARU; // = &s_RIKIMARU;
extern char *D_80089EF0;
extern char *D_80097C60;

typedef struct
{
    char *choice_name;
    u32 choice_number;
} debug_menu_choice;

// int debug_menu_choose(char *screen_header, debug_menu_choice *choices, char *param_3);