#include "gfx.h"

tsgl_sprite* gfx_loadSprite(const char* path) {
    return tsgl_bmp_load(path, colormode, BUFFER, TRANSPARENT_RAWCOLOR);
}


void gfx_image(tsgl_pos x, tsgl_pos y, const char* path) {
    tsgl_sprite* sprite = gfx_loadSprite(path);
    if (!sprite) return;
    PUSH_FUNC(&framebuffer, x, y, sprite);
    tsgl_sprite_free(sprite);
}

void gfx_drawCenteredImage(tsgl_pos x, tsgl_pos y, const char* path) {
    tsgl_sprite* sprite = gfx_loadSprite(path);
    if (!sprite) return;
    PUSH_FUNC(&framebuffer, x - (sprite->fb->width / 2), y - (sprite->fb->height / 2), sprite);
    tsgl_sprite_free(sprite);
}

void gfx_drawCenteredImageWithTransparentSupport(tsgl_pos x, tsgl_pos y, const char* path) {
    tsgl_sprite* sprite = gfx_loadSprite(path);
    if (!sprite) return;
    PUSH_FUNC_TRANS(&framebuffer, x - (sprite->fb->width / 2), y - (sprite->fb->height / 2), sprite);
    tsgl_sprite_free(sprite);
}

void gfx_drawCenteredScreenImage(const char* path) {
    gfx_drawCenteredImage(
        width / 2,
        height / 2,
        path
    );
}


void gfx_imageWithTransparentSupport(tsgl_pos x, tsgl_pos y, const char* path) {
    tsgl_sprite* sprite = gfx_loadSprite(path);
    if (!sprite) return;
    PUSH_FUNC_TRANS(&framebuffer, x, y, sprite);
    tsgl_sprite_free(sprite);
}

void gfx_drawCenteredImageSprite(tsgl_pos x, tsgl_pos y, tsgl_sprite* sprite) {
    PUSH_FUNC(&framebuffer, x - (sprite->fb->width / 2), y - (sprite->fb->height / 2), sprite);
}

void gfx_drawCenteredImageSpriteWithTransparentSupport(tsgl_pos x, tsgl_pos y, tsgl_sprite* sprite) {
    PUSH_FUNC_TRANS(&framebuffer, x - (sprite->fb->width / 2), y - (sprite->fb->height / 2), sprite);
}

void gfx_drawCenteredScreenImageSprite(tsgl_sprite* sprite) {
    gfx_drawCenteredImageSprite(
        width / 2,
        height / 2,
        sprite
    );
}

