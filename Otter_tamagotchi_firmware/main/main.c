#include "main.h"

#define TARGET_FPS 20

static const char* TAG = "otter_tamagotchi";

static tsgl_display display;
static tsgl_framebuffer framebuffer;

static tsgl_pos width;
static tsgl_pos height;
static tsgl_colormode colormode;

static tsgl_rawcolor white;
static tsgl_rawcolor black;

static void drawCenteredImage(const char* path) {
    tsgl_sprite* sprite = tsgl_bmp_load(path, colormode, BUFFER, tsgl_color_raw(tsgl_color_pack(0, 125, 0), colormode));

    tsgl_framebuffer_push(&framebuffer,
        (width / 2) - (sprite->sprite->width / 2),
        (height / 2) - (sprite->sprite->height / 2),
        sprite
    );

    tsgl_bmp_free(sprite);
}

static void bootlogo() {
    tsgl_framebuffer_clear(&framebuffer, white);
    drawCenteredImage("/storage/bootlogo.bmp");
}

void app_main() {
    colormode = settings.driver->colormode;

    white = tsgl_color_raw(TSGL_WHITE, colormode);
    black = tsgl_color_raw(TSGL_BLACK, colormode);

    settings.backlight_init = true;
    settings.backlight_pin = BL;
    settings.backlight_value = 255;

    ESP_ERROR_CHECK(tsgl_filesystem_mount_fatfs("/storage", "storage"));

    ESP_ERROR_CHECK(tsgl_framebuffer_init(&framebuffer, colormode, settings.width, settings.height, BUFFER));
    tsgl_framebuffer_rotate(&framebuffer, FRAMEBUFFER_ROTATE);
    width = framebuffer.width;
    height = framebuffer.height;

    bootlogo();

    settings.init_state = tsgl_display_init_framebuffer;
    settings.init_framebuffer_ptr = framebuffer.buffer;
    settings.init_framebuffer_size = framebuffer.buffersize;

    ESP_ERROR_CHECK(tsgl_spi_initManual(framebuffer.buffersize, SPI, SPI_MOSI, SPI_MISO, SPI_CLK));
    ESP_ERROR_CHECK(tsgl_display_spi(&display, settings, SPI, FREQ, DC, CS, RST));

    tsgl_delay(5000);
    while (true) {
        tsgl_display_send(&display, &framebuffer);
        tsgl_delay(5000);
    }
}