#include "game.h"
#include "../gfx.h"

void game_deadscreen_drawAndProcess() {
    tsgl_framebuffer_clear(&framebuffer, black);
    gfx_drawCenteredImageWithTransparentSupport(WIDTH / 2, HEIGHT / 2, "/firmware/images/dead.bmp");
}
