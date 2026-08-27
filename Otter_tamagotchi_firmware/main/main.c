#include "main.h"
#include "gfx.h"
#include "hctl.h"
#include "game/game.h"

#define TARGET_FPS 20

const char* TAG = "otter_tamagotchi";

tsgl_display display;
tsgl_framebuffer framebuffer;

tsgl_pos width;
tsgl_pos height;
tsgl_colormode colormode;

tsgl_rawcolor white;
tsgl_rawcolor orange;
tsgl_rawcolor red;
tsgl_rawcolor green;
tsgl_rawcolor yellow;
tsgl_rawcolor blue;
tsgl_rawcolor transparent;
tsgl_rawcolor black;

tsgl_print_settings printsettings_title = {
    .alignment = tsgl_print_alignment_center,
    .locationMode = tsgl_print_start_top,
    .multiline = true,
    .globalCentering = true,

    .font = font,
    .targetWidth = TITLE_WIDTH,
    .targetHeight = TITLE_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static void bootlogo() {
    tsgl_framebuffer_clear(&framebuffer, black);
    gfx_drawCenteredScreenImage("/storage/bootlogo.bmp");
    tsgl_framebuffer_text(&framebuffer, 0, height - TITLE_HEIGHT - TITLE_MARGIN, printsettings_title, "Otter");
}

void app_main() {
    esp_log_level_set("TSGL_bmp", ESP_LOG_WARN);
    esp_log_level_set("TSGL_framebuffer", ESP_LOG_WARN);

    colormode = settings.driver->colormode;

    white = tsgl_color_raw(TSGL_WHITE, colormode);
    orange = tsgl_color_raw(TSGL_ORANGE, colormode);
    red = tsgl_color_raw(TSGL_RED, colormode);
    green = tsgl_color_raw(TSGL_GREEN, colormode);
    yellow = tsgl_color_raw(TSGL_YELLOW, colormode);
    blue = tsgl_color_raw(TSGL_BLUE, colormode);
    transparent = tsgl_color_raw(TRANSPARENT_COLOR, colormode);
    black = tsgl_color_raw(TSGL_BLACK, colormode);

    printsettings_title.fg = tsgl_color_raw(TSGL_RED, colormode);

    settings.backlight_init = true;
    settings.backlight_pin = BL;
    settings.backlight_value = 0;

    ESP_ERROR_CHECK(tsgl_filesystem_mount_fatfs("/storage", "storage"));

    ESP_ERROR_CHECK(tsgl_framebuffer_init(&framebuffer, colormode, settings.width, settings.height, BUFFER));
    if (USE_HARDWARE_ROTATE) {
        tsgl_framebuffer_hardwareRotate(&framebuffer, ROTATE);
    } else {
        tsgl_framebuffer_rotate(&framebuffer, ROTATE);
    }
    width = framebuffer.width;
    height = framebuffer.height;

    printsettings_title.width = width;
    printsettings_title.height = TITLE_HEIGHT;

    bootlogo();

    settings.init_state = tsgl_display_init_framebuffer;
    settings.init_framebuffer_ptr = framebuffer.buffer;
    settings.init_framebuffer_size = framebuffer.buffersize;
    if (USE_HARDWARE_ROTATE) {
        settings.init_framebuffer_rotation = ROTATE;
    }

    ESP_ERROR_CHECK(tsgl_spi_initManual(framebuffer.buffersize, SPI, SPI_MOSI, SPI_MISO, SPI_CLK));
    ESP_ERROR_CHECK(tsgl_display_spi(&display, settings, SPI, FREQ, DC, CS, RST));
    tsgl_display_incompleteSending(&display, false, NULL);
    if (USE_HARDWARE_ROTATE) {
        tsgl_display_rotate(&display, ROTATE);
    }

    hctl_init();

    tsgl_delay(100);
    hctl_setBacklight(BACKLIGHT_MAX);
    while (hctl_isBacklightChangeProcess()) tsgl_delay(50);
    tsgl_delay(3000);
    game_start();
}