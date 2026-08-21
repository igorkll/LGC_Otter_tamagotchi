#include <TSGL_drivers/st77XX.h>

#define SPI TSGL_HOST1
#define FREQ 60000000
#define BUFFER TSGL_RAM
#define DC 12
#define CS TSGL_HOST1_CS
#define RST -1
#define BL 13

tsgl_display_settings settings = {
    .driver = &st77XX_rgb565,
    .swapRGB = true,
    .flipX = true,
    .width = 128,
    .height = 160,
    .offsetY = 32
};
