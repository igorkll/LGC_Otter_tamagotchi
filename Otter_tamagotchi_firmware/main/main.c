#include "main.h"
#include "gfx.h"
#include "hctl.h"
#include "game/game.h"
#include "funcs.h"
#include "pushsound.h"

#define TITLE_WIDTH 8
#define TITLE_HEIGHT 16
#define TITLE_MARGIN 8

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
    .multiline = true,
    .width = WIDTH - (TITLE_MARGIN * 2),
    .height = HEIGHT - (TITLE_MARGIN * 2),
    .globalAlignmentX = tsgl_print_alignment_center,
    .globalAlignmentY = tsgl_print_alignment_right,
    .alignment = tsgl_print_alignment_center,

    // start from bottom
    .locationMode = tsgl_print_start_top,
    .linesRevers = false,
    
    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = TITLE_WIDTH,
    .targetHeight = TITLE_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,
    .stroke_thickness = 1,
    .stroke_no_clamp = true
};

static void bootlogo(int index, const char* title) {
    tsgl_framebuffer_clear(&framebuffer, black);
    char path[MAX_PATH_LEN];
    slnprintf(path, MAX_PATH_LEN, "/storage/bootlogo/bgrt%i.bmp", index);
    gfx_drawCenteredScreenImage(path);

    #ifdef DEBUG_TITLE
        printsettings_title.fill = yellow;
        printsettings_title.fg = green;
        printsettings_title.bg = blue;

        tsgl_print_textArea textArea = tsgl_framebuffer_text(&framebuffer, TITLE_MARGIN, TITLE_MARGIN, printsettings_title, title);
        printf("tsgl_framebuffer_text %i %i %i %i (%i %i)\n", textArea.top, textArea.bottom, textArea.left, textArea.right, textArea.width, textArea.height);
    #else
        tsgl_framebuffer_text(&framebuffer, TITLE_MARGIN, TITLE_MARGIN, printsettings_title, title);
    #endif
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

    printsettings_title.fg = tsgl_color_raw(TSGL_WHITE, colormode);
    printsettings_title.stroke = tsgl_color_raw(TSGL_BLACK, colormode);

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

    hctl_setBacklightAndWait(BACKLIGHT_MAX);
    tsgl_delay(STARTUP_IMAGE_CHANGE_DELAY);

    hctl_setBacklightAndWait(BACKLIGHT_OFF);
    bootlogo(1, "\xCF\xF0\xEE\xE3\xF0\xE0\xEC\xEC\xE8\xF1\xF2\xF1\xEA\xE8\xE5\n\xCB\xE0\xEF\xEA\xE8\n\x55\x77\x55"); // Программистские Лапки UwU
    tsgl_display_send(&display, &framebuffer);
    tsgl_delay(100);

    hctl_setBacklightAndWait(BACKLIGHT_MAX);
    tsgl_delay(STARTUP_IMAGE_CHANGE_DELAY);

    hctl_setBacklightAndWait(BACKLIGHT_OFF);
    tsgl_delay(STARTUP_IMAGE_CHANGE_DELAY);

    game_start();
}