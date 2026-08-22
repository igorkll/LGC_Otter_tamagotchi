#pragma once
#include "main.h"

#define TRANSPARENT_COLOR tsgl_color_pack(254, 125, 33)

void gfx_image(tsgl_pos x, tsgl_pos y, const char* path);
void gfx_drawCenteredImage(tsgl_pos x, tsgl_pos y, const char* path);
void gfx_drawCenteredScreenImage(const char* path);
