#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <esp_heap_caps.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>

#include <TSGL.h>
#include <TSGL_benchmark.h>
#include <TSGL_framebuffer.h>
#include <TSGL_filesystem.h>
#include <TSGL_display.h>
#include <TSGL_color.h>
#include <TSGL_spi.h>
#include <TSGL_bmp.h>

#include "display.h"
#include "config.h"

#define TARGET_FPS 20

static const char* TAG = "otter_tamagotchi";

static tsgl_display display;
static tsgl_framebuffer framebuffer;

static tsgl_rawcolor white;
static tsgl_rawcolor red;
static tsgl_rawcolor black;

void drawCenteredImage(const char* path) {
    tsgl_sprite* sprite = tsgl_bmp_load(path, settings.driver->colormode, BUFFER, tsgl_color_raw(tsgl_color_pack(0, 125, 0), display.colormode));

    tsgl_framebuffer_push(&framebuffer,
        (display.width / 2) - (sprite->sprite->width / 2),
        (display.height / 2) - (sprite->sprite->height / 2),
        sprite
    );

    tsgl_bmp_free(sprite);
}

void app_main() {
    tsgl_colormode colormode = settings.driver->colormode;

    white = tsgl_color_raw(TSGL_WHITE, colormode);
    red = tsgl_color_raw(TSGL_RED, colormode);
    black = tsgl_color_raw(TSGL_BLACK, colormode);

    settings.backlight_init = true;
    settings.backlight_pin = BL;
    settings.backlight_value = 255;

    ESP_ERROR_CHECK(tsgl_filesystem_mount_fatfs("/storage", "storage"));

    ESP_ERROR_CHECK(tsgl_framebuffer_init(&framebuffer, colormode, settings.width, settings.height, BUFFER));
    tsgl_framebuffer_rotate(&framebuffer, FRAMEBUFFER_ROTATE);
    size_t framebuffer_size = settings.width * settings.height * tsgl_colormodeSizes[colormode];

    tsgl_framebuffer_clear(&framebuffer, white);
    drawCenteredImage("/storage/test.bmp");
    tsgl_framebuffer_fill(&framebuffer, 4, 4, 16, 16, tsgl_color_raw(TSGL_RED, display.colormode));

    settings.init_state = tsgl_display_init_framebuffer;
    settings.init_framebuffer_ptr = framebuffer.buffer;
    settings.init_framebuffer_size = framebuffer_size;

    ESP_ERROR_CHECK(tsgl_spi_initManual(framebuffer_size, SPI, SPI_MOSI, SPI_MISO, SPI_CLK));
    ESP_ERROR_CHECK(tsgl_display_spi(&display, settings, SPI, FREQ, DC, CS, RST));

    while (true) {
        tsgl_display_send(&display, &framebuffer);
        tsgl_delay(5000);
    }
}