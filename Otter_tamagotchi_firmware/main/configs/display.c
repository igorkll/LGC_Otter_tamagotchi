#include "display.h"

tsgl_display_settings settings = {
    .driver = &st7735_rgb666,
    .swapRGB = false,
    .width = WIDTH,
    .height = HEIGHT,
    .swapXY = false,
    .flipX = true,
    .flipY = true,
    .offsetX = 0,
    .offsetY = 0
};