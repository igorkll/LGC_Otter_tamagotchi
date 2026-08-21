#include <TSGL_drivers/st77XX.h>

#define FREQ 60000000
#define BUFFER TSGL_RAM

#define RST -1

#define SPI TSGL_HOST1
#define SPI_MOSI 4
#define SPI_MISO -1
#define SPI_CLK 8

#define DC 5
#define CS 6
#define BL 7

#define FRAMEBUFFER_ROTATE 0

tsgl_display_settings settings = {
    .driver = &st77XX_rgb565,
    .swapRGB = false,
    .width = 160,
    .height = 128,
    .swapXY = true,
    .flipY = true,
    .offsetX = 0,
    .offsetY = 0
};
