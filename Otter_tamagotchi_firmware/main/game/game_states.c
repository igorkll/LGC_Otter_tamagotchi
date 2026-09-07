#include "game_states.h"
#include "game.h"
#include "../pushsound.h"
#include "../funcs.h"

#define STATES_MARGIN_LEFT_RIGHT 10
#define STATES_MARGIN_TOP_BOTTOM 20
#define STATES_WIDTH (WIDTH - (STATES_MARGIN_LEFT_RIGHT * 2))
#define STATES_HEIGHT (HEIGHT - (STATES_MARGIN_TOP_BOTTOM * 2))
#define STATES_BORDER_SIZE 2
#define STATES_CONTENT_OFFSET (STATES_BORDER_SIZE + 2)
#define STATES_CONTENT_WIDTH (STATES_WIDTH - (STATES_CONTENT_OFFSET * 2))

#define STATES_FONT_TARGET_WIDTH 8
#define STATES_FONT_TARGET_HEIGHT 8

#define STATES_SLIDER_HEIGHT 6
#define STATES_SLIDER_BORDER_SIZE 1
#define STATES_SLIDER_FILL_OFFSET (STATES_SLIDER_BORDER_SIZE + 1)

#define STATES_GAP 2

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,
    
    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_bottom,
    .targetWidth = STATES_FONT_TARGET_WIDTH,
    .targetHeight = STATES_FONT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static tsgl_pos drawstate_str(tsgl_pos x, tsgl_pos y, const char* text) {
    printsettings.fg = green;
    tsgl_framebuffer_text(&framebuffer, x, y, printsettings, text);
    return y + STATES_FONT_TARGET_HEIGHT + STATES_GAP;
}

static tsgl_pos drawstate_num(tsgl_pos x, tsgl_pos y, const char* title, int value) {
    char text[MAX_ACTION_LEN];
    slnprintf(text, MAX_ACTION_LEN, "%s: %i", title, value);
    return drawstate_str(x, y, text);
}

static void raw_draw_slider(tsgl_pos x, tsgl_pos y, game_state_val value) {
    game_state_val floatValue = (game_state_val)value / 100.0;

    tsgl_framebuffer_rect(&framebuffer, 
        x,
        y,
        STATES_CONTENT_WIDTH,
        STATES_SLIDER_HEIGHT,
        blue,
        STATES_SLIDER_BORDER_SIZE
    );
    
    tsgl_framebuffer_fill(&framebuffer,
        x + STATES_SLIDER_FILL_OFFSET,
        y + STATES_SLIDER_FILL_OFFSET,
        floatValue * (STATES_CONTENT_WIDTH - (STATES_SLIDER_FILL_OFFSET * 2)),
        STATES_SLIDER_HEIGHT - (STATES_SLIDER_FILL_OFFSET * 2),
        green
    );
}

static tsgl_pos drawstate_slider(tsgl_pos x, tsgl_pos y, const char* title, game_state_val value) {
    drawstate_str(x, y, title);
    tsgl_pos slider_pos = y + STATES_FONT_TARGET_HEIGHT + STATES_GAP;
    raw_draw_slider(x, slider_pos, value);
    return slider_pos + STATES_SLIDER_HEIGHT + STATES_GAP;
}

void game_states_draw() {
    if (!current_state.states_opened) return;

    tsgl_pos x = (WIDTH / 2) - (STATES_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (STATES_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, STATES_WIDTH, STATES_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, STATES_WIDTH, STATES_HEIGHT, white, STATES_BORDER_SIZE);

    tsgl_pos x2 = STATES_CONTENT_OFFSET + x;
    tsgl_pos y2 = STATES_CONTENT_OFFSET + y;
    y2 = drawstate_num(x2, y2, "MONEY", current_state.states_money);
    y2 += STATES_FONT_TARGET_HEIGHT;
    y2 = drawstate_slider(x2, y2, "\xF3\xF1\xF2\xE0\xEB\xEE\xF1\xF2\xFC", current_state.states_fatigue); //усталость
    y2 = drawstate_slider(x2, y2, "\xE3\xEE\xEB\xEE\xE4", current_state.states_hunger); //голод
    y2 = drawstate_slider(x2, y2, "\xE6\xE0\xE6\xE4\xE0", current_state.states_thirst); //жажда
    y2 = drawstate_slider(x2, y2, "\xED\xE5\xE6\xED\xEE\xF1\xF2\xFC", current_state.states_caress); //нежность
    y2 = drawstate_slider(x2, y2, "\xEF\xE5\xF7\xE0\xEB\xFC", current_state.states_sadness); //печаль
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

void game_states_change(game_state_val* ptr, game_state_val delta) {
    *ptr += delta;
    if (*ptr < 0) *ptr = 0;
    if (*ptr > 100) *ptr = 100;
}
