#include "main.h"
#include "gfx.h"
#include "hctl.h"
#include "game/game.h"
#include "funcs.h"
#include "pushsound.h"

#define TARGET_FPS 20
#define DEBUG_TEXT true

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
    .locationMode = tsgl_print_start_bottom,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .multiline = true,
    .globalCentering = true,

    .font = DejaVuSerif,
    .targetWidth = TITLE_WIDTH,
    .targetHeight = TITLE_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static void bootlogo(int index, const char* title) {
    tsgl_framebuffer_clear(&framebuffer, black);
    char path[MAX_PATH_LEN];
    slnprintf(path, MAX_PATH_LEN, "/storage/bootlogo/bgrt%i.bmp", index);
    gfx_drawCenteredScreenImage(path);

    if (DEBUG_TEXT) {
        tsgl_print_locationMode _old = printsettings_title.locationMode;

        printsettings_title.locationMode = tsgl_print_start_bottom;
        tsgl_print_textArea textArea = tsgl_framebuffer_text(&framebuffer, 0, height - TITLE_HEIGHT - TITLE_MARGIN - 60, printsettings_title, title);
        tsgl_framebuffer_fill(&framebuffer, textArea.left, textArea.top, textArea.width, textArea.height, yellow);
        tsgl_framebuffer_text(&framebuffer, 0, height - TITLE_HEIGHT - TITLE_MARGIN - 60, printsettings_title, title);

        printf("tsgl_framebuffer_text 1 %i %i\n", textArea.width, textArea.height);

        printsettings_title.locationMode = _old;
        textArea = tsgl_framebuffer_text(&framebuffer, 0, height - TITLE_HEIGHT - TITLE_MARGIN, printsettings_title, title);
        tsgl_framebuffer_fill(&framebuffer, textArea.left, textArea.top, textArea.width, textArea.height, yellow);

        printf("tsgl_framebuffer_text 2 %i %i\n", textArea.width, textArea.height);
    }

    tsgl_framebuffer_text(&framebuffer, 0, height - TITLE_HEIGHT - TITLE_MARGIN, printsettings_title, title);

}

static void setBacklightAndWait(uint8_t value) {
    hctl_setBacklight(value);
    while (hctl_isBacklightChangeProcess()) tsgl_delay(50);
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

    bootlogo(0, "\xCC\xE8\xED\xE8\x20\xC2\xFB\xE4\xF0\xE0\x20\x3E\x3C"); // "Мини Выдра ><"

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

    pushsound_play("/storage/bootlogo/startup.pcm", 8000);

    setBacklightAndWait(BACKLIGHT_MAX);
    tsgl_delay(3000);

    setBacklightAndWait(BACKLIGHT_OFF);
    bootlogo(1, "1\n2\n3");
    tsgl_display_send(&display, &framebuffer);
    tsgl_delay(100);

    setBacklightAndWait(BACKLIGHT_MAX);
    tsgl_delay(3000);

    game_start();
}