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

tsgl_display_settings settings = {
    .driver = &st77XX_rgb565,
    .swapRGB = true,
    .flipX = true,
    .width = 128,
    .height = 160,
    .offsetY = 32
};
