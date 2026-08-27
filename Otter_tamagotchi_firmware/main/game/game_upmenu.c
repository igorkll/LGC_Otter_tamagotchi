#include "game_upmenu.h"
#include "../gfx.h"

static tsgl_sprite* sprites[GAME_UPMENU_COUNTS_COUNT];
static tsgl_sprite* sprite_iconline;

void game_upmenu_init() {
    for (size_t i = 0; i < GAME_UPMENU_COUNTS_COUNT; i++) {
        char path[40] = "";
        sprintf(path, "/storage/icons/%i.bmp", i);
        tsgl_sprite* sprite = gfx_loadSprite(path);
        if (sprite == NULL) {
            sprite = gfx_loadSprite("/storage/icons/null.bmp");
        }
        sprites[i] = sprite;
    }
    sprite_iconline = gfx_loadSprite("/storage/images/iconline.bmp");
}

void game_upmenu_process() {

}

void game_upmenu_draw() {
    int lineHeight = sprite_iconline->sprite->height;

    tsgl_framebuffer_push(&framebuffer, 0, 0, sprite_iconline);
    tsgl_framebuffer_push(&framebuffer, 0, framebuffer.height - lineHeight, sprite_iconline);
    
    for (size_t i = 0; i < GAME_UPMENU_LINE_COUNTS_COUNT; i++) {
        int iconWidth = sprites[0]->sprite->width;
        int iconHeight = sprites[0]->sprite->height;
        tsgl_framebuffer_push(&framebuffer, (i * (width / 5)), (lineHeight / 2) - (iconHeight / 2), sprites[i]);
    }
}