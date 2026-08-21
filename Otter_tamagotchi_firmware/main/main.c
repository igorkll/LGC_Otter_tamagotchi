#include "main.h"
#include "gfx.h"

#define TARGET_FPS 20

const char* TAG = "otter_tamagotchi";

tsgl_display display;
tsgl_framebuffer framebuffer;

tsgl_pos width;
tsgl_pos height;
tsgl_colormode colormode;

tsgl_rawcolor white;
tsgl_rawcolor black;

tsgl_print_settings printsettings_title = {
    .alignment = tsgl_print_alignment_center,
    .font = font,
    .targetWidth = 16,
    .targetHeight = 16,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static void bootlogo() {
    tsgl_framebuffer_clear(&framebuffer, black);
    gfx_drawCenteredImage("/storage/bootlogo.bmp");
    tsgl_framebuffer_text(&framebuffer, 0, height - 16, printsettings_title, "Otter");
}

void app_main() {
    colormode = settings.driver->colormode;

    white = tsgl_color_raw(TSGL_WHITE, colormode);
    black = tsgl_color_raw(TSGL_BLACK, colormode);

    printsettings_title.fg = tsgl_color_raw(TSGL_RED, colormode);

    settings.backlight_init = true;
    settings.backlight_pin = BL;
    settings.backlight_value = 255;

    ESP_ERROR_CHECK(tsgl_filesystem_mount_fatfs("/storage", "storage"));

    ESP_ERROR_CHECK(tsgl_framebuffer_init(&framebuffer, colormode, settings.width, settings.height, BUFFER));
    if (USE_HARDWARE_ROTATE) {
        tsgl_framebuffer_hardwareRotate(&framebuffer, ROTATE);
    } else {
        tsgl_framebuffer_rotate(&framebuffer, ROTATE);
    }
    width = framebuffer.width;
    height = framebuffer.height;

    bootlogo();

    settings.init_state = tsgl_display_init_framebuffer;
    settings.init_framebuffer_ptr = framebuffer.buffer;
    settings.init_framebuffer_size = framebuffer.buffersize;
    if (USE_HARDWARE_ROTATE) {
        settings.init_framebuffer_rotation = ROTATE;
    }

    ESP_ERROR_CHECK(tsgl_spi_initManual(framebuffer.buffersize, SPI, SPI_MOSI, SPI_MISO, SPI_CLK));
    ESP_ERROR_CHECK(tsgl_display_spi(&display, settings, SPI, FREQ, DC, CS, RST));
    if (USE_HARDWARE_ROTATE) {
        tsgl_display_rotate(&display, ROTATE);
    }

    tsgl_delay(5000);
    while (true) {
        tsgl_display_send(&display, &framebuffer);
        tsgl_delay(5000);
    }
}