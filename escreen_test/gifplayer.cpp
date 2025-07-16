#include "common.h"
#include <AnimatedGIF.h>

static AnimatedGIF _gif;

void LocalGifDraw(GIFDRAW *pDraw)
{
  // if (_gifPlayer.drawType == GIF_DRAW_COOKED)
  {
    if (pDraw->y == 0) {
      SCREEN_SetAddrWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
    }

    SCREEN_PushPixelsDMA((uint16_t *)pDraw->pPixels, pDraw->iWidth);
  }
}

void GIF_Open()
{

}

void GIF_PlayFromRAM(uint8_t *buffer, size_t buff_len)
{
  if (_gif.open(buffer, buff_len, LocalGifDraw)) {

  }
}