#pragma once
#include "main.h"

//#define TRANSPARENT_COLOR tsgl_color_pack(254, 125, 33)
//#define TRANSPARENT_COLOR tsgl_color_pack(160, 160, 160)
#define TRANSPARENT_COLOR tsgl_color_pack(0, 0, 0)

tsgl_sprite* gfx_loadSprite(const char* path);

void gfx_image(tsgl_pos x, tsgl_pos y, const char* path);
void gfx_drawCenteredImage(tsgl_pos x, tsgl_pos y, const char* path);
void gfx_drawCenteredScreenImage(const char* path);

void gfx_drawCenteredImageSprite(tsgl_pos x, tsgl_pos y, tsgl_sprite* sprite);
void gfx_drawCenteredScreenImageSprite(tsgl_sprite* sprite);
