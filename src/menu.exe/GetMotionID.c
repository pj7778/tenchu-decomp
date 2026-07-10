/* GetMotionID is byte-identical in main.exe and menu.exe -- it uses no
 * relocations, so the same source links unchanged in both. Share the C
 * rather than copying it; cpp resolves this relative to THIS file, and
 * -MMD records it so an edit to the original rebuilds menu.exe too. */
#include "../main.exe/GetMotionID.c"
