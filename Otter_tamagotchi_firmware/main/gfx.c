#include "gfx.h"

void gfx_image(tsgl_pos x, tsgl_pos y, const char* path) {
    tsgl_sprite* sprite = tsgl_bmp_load(path, colormode, BUFFER, tsgl_color_raw(TRANSPARENT_COLOR, colormode));
    tsgl_framebuffer_push(&framebuffer, x, y, sprite);
    tsgl_bmp_free(sprite);
}

void gfx_drawCenteredImage(const char* path) {
    tsgl_sprite* sprite = tsgl_bmp_load(path, colormode, BUFFER, tsgl_color_raw(TRANSPARENT_COLOR, colormode));

    tsgl_framebuffer_push(&framebuffer,
        (width / 2) - (sprite->sprite->width / 2),
        (height / 2) - (sprite->sprite->height / 2),
        sprite
    );

    tsgl_bmp_free(sprite);
}
