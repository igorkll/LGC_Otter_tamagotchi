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

#define SETTINGS_SLIDER_HEIGHT 6
#define SETTINGS_SLIDER_BORDER_SIZE 1
#define SETTINGS_SLIDER_FILL_OFFSET (SETTINGS_SLIDER_BORDER_SIZE + 1)

#define SETTINGS_GAP 2

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

static void raw_draw_slider(tsgl_pos x, tsgl_pos y, game_state_val value) {
    game_state_val floatValue = (game_state_val)value / 100.0;

    tsgl_framebuffer_rect(&framebuffer, 
        x,
        y,
        SETTINGS_CONTENT_WIDTH,
        SETTINGS_SLIDER_HEIGHT,
        blue,
        SETTINGS_SLIDER_BORDER_SIZE
    );
    
    tsgl_framebuffer_fill(&framebuffer,
        x + SETTINGS_SLIDER_FILL_OFFSET,
        y + SETTINGS_SLIDER_FILL_OFFSET,
        floatValue * (SETTINGS_CONTENT_WIDTH - (SETTINGS_SLIDER_FILL_OFFSET * 2)),
        SETTINGS_SLIDER_HEIGHT - (SETTINGS_SLIDER_FILL_OFFSET * 2),
        green
    );
}

static tsgl_pos drawstate_slider(tsgl_pos x, tsgl_pos y, const char* title, game_state_val value) {
    drawstate_str(x, y, title);
    tsgl_pos slider_pos = y + SETTINGS_FONT_TARGET_HEIGHT + SETTINGS_GAP;
    raw_draw_slider(x, slider_pos, value);
    return slider_pos + SETTINGS_SLIDER_HEIGHT + SETTINGS_GAP;
}

void game_settings_draw() {
    if (!current_state.settings_opened) return;

    tsgl_pos x = (WIDTH / 2) - (SETTINGS_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (SETTINGS_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, SETTINGS_WIDTH, SETTINGS_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, SETTINGS_WIDTH, SETTINGS_HEIGHT, white, SETTINGS_BORDER_SIZE);

    tsgl_pos x2 = SETTINGS_CONTENT_OFFSET + x;
    tsgl_pos y2 = SETTINGS_CONTENT_OFFSET + y;
    y2 = drawstate_slider(x2, y2, "1", current_state.settings_master_volume);
    y2 = drawstate_slider(x2, y2, "2", current_state.settings_music_volume);
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
        game_settings_open();
    } else {
        game_settings_close();
    }
}
