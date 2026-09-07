#include "game_states.h"
#include "game.h"
#include "../pushsound.h"
#include "../funcs.h"

#define STATES_MARGIN_LEFT_RIGHT 20
#define STATES_MARGIN_TOP_BOTTOM 40
#define STATES_WIDTH (WIDTH - (STATES_MARGIN_LEFT_RIGHT * 2))
#define STATES_HEIGHT (HEIGHT - (STATES_MARGIN_TOP_BOTTOM * 2))
#define STATES_BORDER_SIZE 2
#define STATES_CONTENT_OFFSET (STATES_BORDER_SIZE + 2)
#define STATES_CONTENT_WIDTH (STATES_WIDTH - (STATES_CONTENT_OFFSET * 2))

#define STATES_FONT_TARGET_WIDTH 8
#define STATES_FONT_TARGET_HEIGHT 8

#define STATES_SLIDER_HEIGHT 4
#define STATES_SLIDER_BORDER_SIZE 1
#define STATES_SLIDER_FILL_OFFSET (STATES_BORDER_SIZE + 1)

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,
    
    // font
    .font = font,
    .localLocationMode = tsgl_print_localLocationMode_bottom,
    .targetWidth = STATES_FONT_TARGET_WIDTH,
    .targetHeight = STATES_FONT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static void drawstate_str(tsgl_pos x, tsgl_pos y, const char* text) {
    printsettings.fg = green;
    tsgl_framebuffer_text(&framebuffer, x, y, printsettings, text);
}

static void drawstate_num(tsgl_pos x, tsgl_pos y, const char* title, int value) {
    char text[MAX_ACTION_LEN];
    slnprintf(text, MAX_ACTION_LEN, "%s: %i", title, value);
    drawstate_str(x, y, text);
}

static void raw_draw_slider(tsgl_pos x, tsgl_pos y, int8_t value) {
    tsgl_framebuffer_rect(&framebuffer, x, y, STATES_CONTENT_WIDTH, STATES_SLIDER_HEIGHT, blue, STATES_SLIDER_BORDER_SIZE);
    tsgl_framebuffer_fill(&framebuffer, x + STATES_SLIDER_FILL_OFFSET, y + STATES_SLIDER_FILL_OFFSET, STATES_CONTENT_WIDTH - (STATES_SLIDER_FILL_OFFSET * 2), STATES_SLIDER_HEIGHT - (STATES_SLIDER_FILL_OFFSET * 2), green);
}

static void drawstate_slider(tsgl_pos x, tsgl_pos y, const char* title, int8_t value) {
    drawstate_str(x, y, title);
}

states_sadness

void game_states_draw() {
    if (!current_state.states_opened) return;

    tsgl_pos x = (WIDTH / 2) - (STATES_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (STATES_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, STATES_WIDTH, STATES_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, STATES_WIDTH, STATES_HEIGHT, white, STATES_BORDER_SIZE);

    drawstate_num(STATES_CONTENT_OFFSET + x, STATES_CONTENT_OFFSET + y, "MONEY", current_state.states_money);
}

void game_states_open() {
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, EFFECTS_SOUND_VOLUME * 2);
    current_state.states_opened = true;
    game_updateActiveIcons();
}

void game_states_close() {
    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, EFFECTS_SOUND_VOLUME * 2);
    current_state.states_opened = false;
    game_updateActiveIcons();
}

void game_states_toggle() {
    if (current_state.states_opened) {
        game_states_close();
    } else {
        game_states_open();
    }
}
