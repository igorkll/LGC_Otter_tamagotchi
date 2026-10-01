#include "game.h"
#include "../gfx.h"

void game_dead_drawAndProcess() {
    tsgl_framebuffer_clear(&framebuffer, black);
    gfx_drawCenteredImageWithTransparentSupport(WIDTH / 2, HEIGHT / 2, "/firmware/images/dead.bmp");
}

void game_dead_gameover() {
    if (current_state.dead) return;
    current_state.dead = true;
    game_closeAltApp();
}
