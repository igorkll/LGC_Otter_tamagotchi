#include "game_backpack.h"
#include "game.h"
#include "../pushsound.h"

#define BACKPACK_MARGIN_LEFT_RIGHT 20
#define BACKPACK_MARGIN_TOP_BOTTOM 40
#define BACKPACK_WIDTH (WIDTH - (BACKPACK_MARGIN_LEFT_RIGHT * 2))
#define BACKPACK_HEIGHT (HEIGHT - (BACKPACK_MARGIN_TOP_BOTTOM * 2))

void game_backpack_draw() {
    if (!current_state.backpack_opened) return;

    tsgl_pos x = (WIDTH / 2) - (BACKPACK_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (BACKPACK_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, BACKPACK_WIDTH, BACKPACK_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, BACKPACK_WIDTH, BACKPACK_HEIGHT, white, 2);
}

void game_backpack_open() {
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, EFFECTS_SOUND_VOLUME * 2);
    current_state.backpack_opened = true;
    game_updateActiveIcons();
}

void game_backpack_close() {
    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, EFFECTS_SOUND_VOLUME * 2);
    current_state.backpack_opened = false;
    game_updateActiveIcons();
}

void game_backpack_toggle() {
    if (current_state.backpack_opened) {
        game_backpack_close();
    } else {
        game_backpack_open();
    }
}
