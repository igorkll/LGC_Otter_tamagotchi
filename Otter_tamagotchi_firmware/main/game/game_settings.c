#include "game_backpack.h"
#include "game.h"
#include "gfx.h"
#include "../pushsound.h"

#define BACKPACK_WIDTH (WIDTH - 20)
#define BACKPACK_HEIGHT (WIDTH - 20)
#define BACKPACK_BORDER_SIZE 2

#define BACKPACK_ICON_SIZE 24
#define BACKPACK_ICON_TEXT_WIDTH 8
#define BACKPACK_ICON_TEXT_HEIGHT 8
#define BACKPACK_ICON_OFFSET (BACKPACK_ICON_SIZE * 1.5)
#define BACKPACK_ICON_TEXT_OFFSET ((BACKPACK_ICON_SIZE / 2) + (BACKPACK_ICON_TEXT_HEIGHT / 2) + 1)
#define BACKPACK_ICON_BORDER_SIZE 1

tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = BACKPACK_ICON_TEXT_WIDTH,
    .targetHeight = BACKPACK_ICON_TEXT_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,
    .stroke_thickness = 1,
    .stroke_no_clamp = true
};

static void draw_icon(tsgl_pos ox, tsgl_pos oy, const char* icon, int count) {
    tsgl_pos x = (WIDTH / 2) + (ox * BACKPACK_ICON_OFFSET);
    tsgl_pos y = (HEIGHT / 2) + (oy * BACKPACK_ICON_OFFSET);
    tsgl_pos cornerX = x - (BACKPACK_ICON_SIZE / 2);
    tsgl_pos cornerY = y - (BACKPACK_ICON_SIZE / 2);

    char path[MAX_PATH_LEN];
    path[0] = '\0';
    TSGL_funcs_slnprintf(path, MAX_PATH_LEN, "/firmware/icons/backpack/%s.bmp", icon);

    char text[MAX_PATH_LEN];
    text[0] = '\0';
    TSGL_funcs_slnprintf(text, MAX_PATH_LEN, "%i", count);

    gfx_drawCenteredImageWithTransparentSupport(x, y, path);
    tsgl_framebuffer_rect(&framebuffer, cornerX, cornerY, BACKPACK_ICON_SIZE, BACKPACK_ICON_SIZE, white, BACKPACK_ICON_BORDER_SIZE);

    printsettings.fg = white;
    tsgl_framebuffer_text(&framebuffer, x - (BACKPACK_ICON_TEXT_WIDTH / 2), y + BACKPACK_ICON_TEXT_OFFSET, printsettings, text);
}

void game_settings_draw() {
    if (!current_state.settings_opened) return;

    tsgl_pos x = (WIDTH / 2) - (BACKPACK_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (BACKPACK_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, BACKPACK_WIDTH, BACKPACK_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, BACKPACK_WIDTH, BACKPACK_HEIGHT, white, BACKPACK_BORDER_SIZE);

    draw_icon(-1, -1, "eat", current_state.backpack_eat_count);
    draw_icon(0, -1, "water", current_state.backpack_water_count);
}

void game_settings_open() {
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.settings_opened = true;
    game_updateActiveIcons();
}

void game_settings_close() {
    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.settings_opened = false;
    game_updateActiveIcons();
}

void game_settings_toggle() {
    if (current_state.settings_opened) {
        game_settings_close();
    } else {
        game_settings_open();
    }
}
