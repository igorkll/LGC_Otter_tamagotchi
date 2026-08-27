#include "game_upmenu.h"
#include "../gfx.h"

static tsgl_sprite* sprites[GAME_UPMENU_COUNTS_COUNT];
static tsgl_sprite* sprite_iconline;

void game_upmenu_init() {
    sprite_iconline = gfx_loadSprite("/storage/images/iconline.bmp");
}

void game_upmenu_process() {

}

void game_upmenu_draw() {
    tsgl_framebuffer_push(&framebuffer, 0, 0, sprite_iconline);
    tsgl_framebuffer_push(&framebuffer, 0, framebuffer.height - sprite_iconline->sprite->height, sprite_iconline);
}