#include "game.h"
#include "../hctl.h"
#include "../gfx.h"
#include <esp_system.h>

#define SELECTTEXT_TARGET_WIDTH 8
#define SELECTTEXT_TARGET_HEIGHT 16
#define TEXT_MARGIN 8

tsgl_print_settings printsettings_selectText = {
    .locationMode = tsgl_print_start_bottom,

    // multiline
    .multiline = true,
    .globalAlignmentX = tsgl_print_alignment_center,
    .alignment = tsgl_print_alignment_center,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_bottom,
    .targetWidth = SELECTTEXT_TARGET_WIDTH,
    .targetHeight = SELECTTEXT_TARGET_HEIGHT,
    .spacing = 3,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static const char* options[] = {
    "\xD1\x20\xCD\xC0\xD7\xC0\xCB\xC0",        // С НАЧАЛА
    "\xC4\xCE\xD1\xD2\xC8\xC6\xC5\xCD\xC8\xDF" // ДОСТИЖЕНИЯ
};

#define OPTIONS_COUNT TSGL_CALC_ARRSIZE(options)

static uint8_t option_selected = 0;

static void resetGame() {
    for (size_t i = 0; i < GAMESTATE_COUNT; i++) {
        tsgl_filesystem_remove(gamestate_paths[i]);
    }

    esp_restart();
}

static void processAction() {
    switch (option_selected) {
        case 0:
            resetGame();
            break;
    }
}

void game_dead_drawAndProcess() {
    // process
    
    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_LEFT)) {
        option_selected--;
        if (option_selected >= OPTIONS_COUNT) option_selected = OPTIONS_COUNT - 1;
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_RIGHT)) {
        option_selected++;
        if (option_selected >= OPTIONS_COUNT) option_selected = 0;
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_OKAY)) {
        processAction();
    }

    // draw
    tsgl_framebuffer_clear(&framebuffer, black);
    gfx_drawCenteredImageWithTransparentSupport(WIDTH / 2, HEIGHT / 2, "/firmware/images/dead.bmp");

    printsettings_selectText.fg = red;
    printsettings_selectText.width = WIDTH;
    printsettings_selectText.height = HEIGHT;
    tsgl_framebuffer_text(&framebuffer, 0, HEIGHT - TEXT_MARGIN, printsettings_selectText, options[option_selected]);
}

void game_dead_gameover() {
    if (current_state.dead) return;
    current_state.dead = true;
    game_closeAltApp();
}
