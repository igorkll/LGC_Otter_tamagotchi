#include "game_combinemenu.h"
#include "game.h"
#include "../pushsound.h"
#include "../funcs.h"

#define COMBINEMENU_MARGIN_LEFT_RIGHT 10
#define COMBINEMENU_MARGIN_TOP_BOTTOM 20
#define COMBINEMENU_WIDTH (WIDTH - (COMBINEMENU_MARGIN_LEFT_RIGHT * 2))
#define COMBINEMENU_HEIGHT (HEIGHT - (COMBINEMENU_MARGIN_TOP_BOTTOM * 2))
#define COMBINEMENU_BORDER_SIZE 2
#define COMBINEMENU_CONTENT_OFFSET (COMBINEMENU_BORDER_SIZE + 2)
#define COMBINEMENU_CONTENT_WIDTH (COMBINEMENU_WIDTH - (COMBINEMENU_CONTENT_OFFSET * 2))

#define COMBINEMENU_FONT_TARGET_WIDTH 8
#define COMBINEMENU_FONT_TARGET_HEIGHT 8

#define COMBINEMENU_SLIDER_HEIGHT 6
#define COMBINEMENU_SLIDER_BORDER_SIZE 1
#define COMBINEMENU_SLIDER_FILL_OFFSET (COMBINEMENU_SLIDER_BORDER_SIZE + 1)

#define COMBINEMENU_GAP 2

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,
    
    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_bottom,
    .targetWidth = COMBINEMENU_FONT_TARGET_WIDTH,
    .targetHeight = COMBINEMENU_FONT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

void game_combinemenu_draw() {
    if (!current_state.combinemenu_opened) return;


}

void game_combinemenu_open() {
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.combinemenu_opened = true;
    game_updateActiveIcons();
}

void game_combinemenu_close() {
    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.combinemenu_opened = false;
    game_updateActiveIcons();
}

void game_combinemenu_toggle() {
    if (current_state.combinemenu_opened) {
        game_combinemenu_close();
    } else {
        game_combinemenu_open();
    }
}
