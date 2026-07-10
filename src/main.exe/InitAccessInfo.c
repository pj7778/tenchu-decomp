#include "common.h"
#include "main.exe.h"

/*
 * InitAccessInfo (0x800194f4, 0x38 bytes) — one-shot setup for the "access"
 * (grapple/climb) HUD meter, called only from main(): fetch archive image 0x2c,
 * bind it to the meter's 214x217 GT4 quad, and zero the meter's charge.
 *
 * AccessImage (0x800bc0c0) is Ghidra's name for the quad; splat auto-named it
 * D_800BC0C0 only because this function's asm was the sole reference, so
 * matching it here drops the auto symbol — hence the explicit
 * `AccessImage = 0x800bc0c0;` added to config/symbols.main.exe.txt (see
 * PathFileRead.c for the same trap). Its concrete type is not pinned by
 * anything we've matched yet; only its address reaches the callee.
 *
 * AccessPower is %gp_rel (a small in the gp window) — same global PrepareAccess.c
 * clears.
 */

extern GsIMAGE *GetImage(s32 id);
extern void SetupImageToPolyGT4(GsIMAGE *image, void *quad, s32 w, s32 h);
extern s32 AccessPower;
extern u8 AccessImage[];

void InitAccessInfo(void)
{
    SetupImageToPolyGT4(GetImage(0x2c), AccessImage, 0xd6, 0xd9);
    AccessPower = 0;
}
