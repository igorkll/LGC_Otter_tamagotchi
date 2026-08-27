#include "gfx.h"

tsgl_sprite* gfx_loadSprite(const char* path) {
    return tsgl_bmp_load(path, colormode, BUFFER, tsgl_color_raw(TRANSPARENT_COLOR, colormode));
}


void gfx_image(tsgl_pos x, tsgl_pos y, const char* path) {
    tsgl_sprite* sprite = gfx_loadSprite(path);
    tsgl_framebuffer_pushFast(&framebuffer, x, y, sprite);
    tsgl_bmp_free(sprite);
}

void gfx_drawCenteredImage(tsgl_pos x, tsgl_pos y, const char* path) {
    tsgl_sprite* sprite = gfx_loadSprite(path);
    tsgl_framebuffer_pushFast(&framebuffer, x - (sprite->sprite->width / 2), y - (sprite->sprite->height / 2), sprite);
    tsgl_bmp_free(sprite);
}

void gfx_drawCenteredScreenImage(const char* path) {
    gfx_drawCenteredImage(
        width / 2,
        height / 2,
        path
    );
}


void gfx_drawCenteredImageSprite(tsgl_pos x, tsgl_pos y, tsgl_sprite* sprite) {
    tsgl_framebuffer_pushFast(&framebuffer, x - (sprite->sprite->width / 2), y - (sprite->sprite->height / 2), sprite);
}

void gfx_drawCenteredScreenImageSprite(tsgl_sprite* sprite) {
    gfx_drawCenteredImageSprite(
        width / 2,
        height / 2,
        sprite
    );
}

