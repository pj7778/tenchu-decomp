#include "common.h"
#include "main.exe.h"

/*
 * DisposeBG (0x8001885c) — free a tile-BG object's three dynamic buffers
 * (`cell`@0x34, `work`@0x38, `index`@0x3C) then the BackGround itself. Same
 * null-check-then-free shape as DisposeAfterimage/DisposeMotionManager, one
 * more field. Ghidra renders the first free as `vfree(&bg->cell->u)` (a
 * union-address artifact of its GsBG/GsMAP struct nesting), but the raw .s
 * is a plain `lw $a0, 0x34($s0)` — a field LOAD, not an address computation
 * — so trust m2c's flat `vfree(arg0->unk34)` reading instead. Local
 * truncated view (pad to the first freed offset, `void *` fields) rather
 * than importing Ghidra's full nested GsBG/GsMAP, unused elsewhere in the
 * codebase (same convention as AfterimageType in ReqItemHappou/ReqItemLaunch).
 */
extern void vfree(void *p);

typedef struct
{
    u8 pad0[0x34];
    void *cell;
    void *work;
    void *index;
} BackGround;

void DisposeBG(BackGround *bg)
{
    if (bg != 0)
    {
        vfree(bg->cell);
        vfree(bg->work);
        vfree(bg->index);
        vfree(bg);
    }
}
