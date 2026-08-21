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
    settings.backlight_init = true;
    settings.backlight_pin = BL;
    settings.backlight_value = 255;

    settings.init_state = tsgl_display_init_color;
    settings.init_color = tsgl_color_raw(TSGL_BLACK, settings.driver->colormode);

    ESP_ERROR_CHECK(tsgl_filesystem_mount_fatfs("/storage", "storage"));
    ESP_ERROR_CHECK(tsgl_spi_init(settings.width * settings.height * tsgl_colormodeSizes[settings.driver->colormode], SPI));
    ESP_ERROR_CHECK(tsgl_display_spi(&display, settings, SPI, FREQ, DC, CS, RST));
    ESP_ERROR_CHECK(tsgl_framebuffer_init(&framebuffer, display.colormode, settings.width, settings.height, BUFFER));

    tsgl_display_incompleteSending(&display, false, &framebuffer);
    tsgl_framebuffer_rotate(&framebuffer, 3);

    white = tsgl_color_raw(TSGL_WHITE, display.colormode);

    while (true) {
        tsgl_framebuffer_clear(&framebuffer, white);
        drawCenteredImage("/storage/text1.bmp");
        tsgl_display_send(&display, &framebuffer);
        tsgl_delay(5000);

        for (int i = 0; i < 5; i++) {
            tsgl_framebuffer_clear(&framebuffer, white);
            drawCenteredImage("/storage/heart1.bmp");
            tsgl_display_send(&display, &framebuffer);
            tsgl_delay(500);

            tsgl_framebuffer_clear(&framebuffer, white);
            drawCenteredImage("/storage/heart2.bmp");
            tsgl_display_send(&display, &framebuffer);
            tsgl_delay(500);
        }

        tsgl_framebuffer_clear(&framebuffer, white);
        drawCenteredImage("/storage/text2.bmp");
        tsgl_display_send(&display, &framebuffer);
        tsgl_delay(5000);
    }
}