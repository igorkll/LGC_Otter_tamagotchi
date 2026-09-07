#include "game_states.h"
#include "game.h"
#include "../pushsound.h"

#define STATES_MARGIN_LEFT_RIGHT 20
#define STATES_MARGIN_TOP_BOTTOM 40
#define STATES_WIDTH (WIDTH - (STATES_MARGIN_LEFT_RIGHT * 2))
#define STATES_HEIGHT (HEIGHT - (STATES_MARGIN_TOP_BOTTOM * 2))

#define STATES_FONT_TARGET_WIDTH 8
#define STATES_FONT_TARGET_HEIGHT 8

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

static void draw_number_state() {
    printsettings.fg = green;


}

void game_states_draw() {
    if (!current_state.states_opened) return;

    tsgl_pos x = (WIDTH / 2) - (STATES_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (STATES_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, STATES_WIDTH, STATES_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, STATES_WIDTH, STATES_HEIGHT, white, 2);
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
