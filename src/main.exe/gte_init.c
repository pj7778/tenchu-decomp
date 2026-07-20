#include "common.h"
#include "main.exe.h"

/* Private libgs state owned by the complete GS_121.OBJ member. */
extern short GsORGOFSX;
extern short GsORGOFSY;

void gte_init(void)
{
    InitGeom();
    SetFarColor(0, 0, 0);
    SetGeomOffset(0, 0);
    GsORGOFSY = 0;
    GsORGOFSX = 0;
}
