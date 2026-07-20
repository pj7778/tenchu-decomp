#include "common.h"
#include "main.exe.h"

void GsGetTimInfo(unsigned long *image, GsIMAGE *tim)
{
    unsigned long *pixel;

    tim->pmode = *image;
    if ((tim->pmode >> 3) & 1) {
        image++;
        pixel = image + (*image >> 2);
        image++;
        tim->cx = ((short *)image)[0];
        tim->cy = ((short *)image)[1];
        image++;
        tim->cw = ((unsigned short *)image)[0];
        tim->ch = ((unsigned short *)image)[1];
        image++;
        tim->clut = image;

        pixel++;
        tim->px = ((short *)pixel)[0];
        tim->py = ((short *)pixel)[1];
        pixel++;
        tim->pw = ((unsigned short *)pixel)[0];
        tim->ph = ((unsigned short *)pixel)[1];
        pixel++;
        tim->pixel = pixel;
    } else {
        image += 2;
        tim->px = ((short *)image)[0];
        tim->py = ((short *)image)[1];
        image++;
        tim->pw = ((unsigned short *)image)[0];
        tim->ph = ((unsigned short *)image)[1];
        image++;
        tim->pixel = image;
    }
}
