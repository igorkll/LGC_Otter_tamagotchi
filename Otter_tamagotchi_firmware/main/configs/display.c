#include "display.h"

tsgl_display_settings settings = {
    .driver = &st77XX_rgb565,
    .swapRGB = false,
    .width = 128,
    .height = 160,
    .swapXY = false,
    .flipX = true,
    .flipY = true,
    .offsetX = 0,
    .offsetY = 0
};