#include "game_states.h"
#include "game.h"
#include "../pushsound.h"
#include "../funcs.h"

#define SETTINGS_MARGIN_LEFT_RIGHT 10
#define SETTINGS_MARGIN_TOP_BOTTOM 20
#define SETTINGS_WIDTH (WIDTH - (SETTINGS_MARGIN_LEFT_RIGHT * 2))
#define SETTINGS_HEIGHT (HEIGHT - (SETTINGS_MARGIN_TOP_BOTTOM * 2))
#define SETTINGS_BORDER_SIZE 2
#define SETTINGS_CONTENT_OFFSET (SETTINGS_BORDER_SIZE + 2)
#define SETTINGS_CONTENT_WIDTH (SETTINGS_WIDTH - (SETTINGS_CONTENT_OFFSET * 2))

#define SETTINGS_FONT_TARGET_WIDTH 8
#define SETTINGS_FONT_TARGET_HEIGHT 8

#define SETTINGS_SLIDER_HEIGHT 8
#define SETTINGS_SLIDER_BORDER_SIZE 1
#define SETTINGS_SLIDER_FILL_OFFSET (SETTINGS_SLIDER_BORDER_SIZE + 1)

#define SETTINGS_GAP 4
#define SETTINGS_COUNT 2

static int8_t current_setting = 0;
static bool setting_lock = false;

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,
    
    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_bottom,
    .targetWidth = SETTINGS_FONT_TARGET_WIDTH,
    .targetHeight = SETTINGS_FONT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static tsgl_pos drawstate_str(tsgl_pos x, tsgl_pos y, const char* text) {
    printsettings.fg = green;
    tsgl_framebuffer_text(&framebuffer, x, y, printsettings, text);
    return y + SETTINGS_FONT_TARGET_HEIGHT + SETTINGS_GAP;
}

static void raw_draw_slider(tsgl_pos x, tsgl_pos y, game_state_val value, bool selected) {
    tsgl_rawcolor rectColor = blue;
    tsgl_rawcolor bodyColor = green;

    if (selected) {
        rectColor = tsgl_color_raw(tsgl_color_pack(255, 40, 40), colormode);
        if (setting_lock) {
            bodyColor = tsgl_color_raw(tsgl_color_pack(64, 255, 255), colormode);
        }
    }

    tsgl_framebuffer_rect(&framebuffer, 
        x,
        y,
        SETTINGS_CONTENT_WIDTH,
        SETTINGS_SLIDER_HEIGHT,
        rectColor,
        SETTINGS_SLIDER_BORDER_SIZE
    );
    
    tsgl_framebuffer_fill(&framebuffer,
        x + SETTINGS_SLIDER_FILL_OFFSET,
        y + SETTINGS_SLIDER_FILL_OFFSET,
        value * (SETTINGS_CONTENT_WIDTH - (SETTINGS_SLIDER_FILL_OFFSET * 2)),
        SETTINGS_SLIDER_HEIGHT - (SETTINGS_SLIDER_FILL_OFFSET * 2),
        bodyColor
    );
}

static tsgl_pos drawstate_slider(tsgl_pos x, tsgl_pos y, const char* title, game_state_val value, bool selected) {
    drawstate_str(x, y, title);
    tsgl_pos slider_pos = y + SETTINGS_FONT_TARGET_HEIGHT + SETTINGS_GAP;
    raw_draw_slider(x, slider_pos, value, selected);
    return slider_pos + SETTINGS_SLIDER_HEIGHT + SETTINGS_GAP;
}

static void float_change(float* ptr, float delta) {
    *ptr += delta / 100;
    if (*ptr < 0) *ptr = 0;
    if (*ptr > 1) *ptr = 1;
    game_updateParameters();
}

static void handle_locked() {
    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_OKAY)) {
        setting_lock = false;
        return;
    }

    float* ptr = 0;
    switch (current_setting) {
        case 0:
            ptr = &current_state.settings_master_volume;
            break;

        case 1:
            ptr = &current_state.settings_music_volume;
            break;
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_LEFT)) {
        float_change(ptr, -5);
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_RIGHT)) {
        float_change(ptr, 5);
    }
}

static void handle_menu() {
    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_LEFT)) {
        current_setting--;
        if (current_setting < 0) current_setting = SETTINGS_COUNT - 1;
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_OKAY)) {
        setting_lock = true;
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_RIGHT)) {
        current_setting++;
        if (current_setting >= SETTINGS_COUNT) current_setting = 0;
    }
}

void game_settings_draw() {
    if (!current_state.settings_opened) return;

    tsgl_pos x = (WIDTH / 2) - (SETTINGS_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (SETTINGS_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, SETTINGS_WIDTH, SETTINGS_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, SETTINGS_WIDTH, SETTINGS_HEIGHT, white, SETTINGS_BORDER_SIZE);

    tsgl_pos x2 = SETTINGS_CONTENT_OFFSET + x;
    tsgl_pos y2 = SETTINGS_CONTENT_OFFSET + y;
    y2 = drawstate_slider(x2, y2, "\xCE\xE1\xF9\xE0\xFF\x20\xE3\xF0\xEE\xEC\xEA\xEE\xF1\xF2\xFC", current_state.settings_master_volume, current_setting == 0);
    y2 = drawstate_slider(x2, y2, "\xC3\xF0\xEE\xEC\xEA\xEE\xF1\xF2\xFC\x20\xEC\xF3\xE7\xFB\xEA\xE8", current_state.settings_music_volume, current_setting == 1);

    if (setting_lock) {
        handle_locked();
    } else {
        handle_menu();
    }
}

void game_settings_open() {
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.settings_opened = true;
    game_updateActiveIcons();
}

void game_settings_close() {
    if (setting_lock) {
        setting_lock = false;
        return;
    }

    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.settings_opened = false;
    game_updateActiveIcons();
}

void game_settings_toggle() {
    if (current_state.settings_opened) {
        game_settings_open();
    } else {
        game_settings_close();
    }
}
