#include "game_achievements.h"
#include "game.h"
#include "gfx.h"
#include "../pushsound.h"

#define ACHIEVEMENTS_WIDTH (WIDTH - 10)
#define ACHIEVEMENTS_HEIGHT (HEIGHT - 10)
#define ACHIEVEMENTS_BORDER_SIZE 2

#define ACHIEVEMENTS_ICON_SIZE 24
#define ACHIEVEMENTS_ICON_TEXT_WIDTH 10
#define ACHIEVEMENTS_ICON_TEXT_HEIGHT 10
#define ACHIEVEMENTS_ICON_ABS_OFFSET_X 0
#define ACHIEVEMENTS_ICON_ABS_OFFSET_Y -6
#define ACHIEVEMENTS_ICON_OFFSET_X (ACHIEVEMENTS_ICON_SIZE * 1.5)
#define ACHIEVEMENTS_ICON_OFFSET_Y (ACHIEVEMENTS_ICON_SIZE * 1.85)
#define ACHIEVEMENTS_ICON_TEXT_OFFSET ((ACHIEVEMENTS_ICON_SIZE / 2) + (ACHIEVEMENTS_ICON_TEXT_HEIGHT / 2) + 1)
#define ACHIEVEMENTS_ICON_BORDER_SIZE 1

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = ACHIEVEMENTS_ICON_TEXT_WIDTH,
    .targetHeight = ACHIEVEMENTS_ICON_TEXT_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

void game_achievements_draw() {
    if (!current_state.achievements_opened) return;

    tsgl_pos x = (WIDTH / 2) - (ACHIEVEMENTS_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (ACHIEVEMENTS_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, ACHIEVEMENTS_WIDTH, ACHIEVEMENTS_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, ACHIEVEMENTS_WIDTH, ACHIEVEMENTS_HEIGHT, white, ACHIEVEMENTS_BORDER_SIZE);
}

void game_achievements_open() {
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, BACKPACK_SOUND_VOLUME);
    current_state.achievements_opened = true;
    game_updateActiveIcons();
}

void game_achievements_close() {
    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, BACKPACK_SOUND_VOLUME);
    current_state.achievements_opened = false;
    game_updateActiveIcons();
}

void game_achievements_toggle() {
    if (current_state.achievements_opened) {
        game_achievements_close();
    } else {
        game_achievements_open();
    }
}
