#include "game_backpack.h"
#include "game.h"

#define BACKPACK_MARGIN_LEFT_RIGHT 20
#define BACKPACK_MARGIN_TOP_BOTTOM 40
#define BACKPACK_WIDTH (WIDTH - (BACKPACK_MARGIN_LEFT_RIGHT * 2))
#define BACKPACK_HEIGHT (HEIGHT - (BACKPACK_MARGIN_TOP_BOTTOM * 2))

void game_backpack_draw() {
    if (!current_state.backpack_opened) return;

    tsgl_pos x = (WIDTH / 2) - (BACKPACK_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (BACKPACK_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, BACKPACK_WIDTH, BACKPACK_HEIGHT, red);
}

void game_backpack_open() {
    current_state.backpack_opened = true;
}

void game_backpack_close() {
    current_state.backpack_opened = false;
}

void game_backpack_toggle() {
    if (current_state.backpack_opened) {
        game_backpack_close();
    } else {
        game_backpack_open();
    }
}
