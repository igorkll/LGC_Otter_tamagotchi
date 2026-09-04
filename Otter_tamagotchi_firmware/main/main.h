#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#include <esp_heap_caps.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>

#include <TSGL.h>
#include <TSGL_benchmark.h>
#include <TSGL_framebuffer.h>
#include <TSGL_filesystem.h>
#include <TSGL_display.h>
#include <TSGL_color.h>
#include <TSGL_spi.h>
#include <TSGL_bmp.h>
#include <TSGL_keyboard.h>
#include <TSGL_sound.h>
#include <TSGL_math.h>
#include <TSGL_fonts/font.h>
#include <TSGL_fonts/dejavu/DejaVuSerif.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include "configs/display.h"
#include "configs/consts.h"
#include "configs/config.h"
#include "configs/gamecfg.h"

extern const char* TAG;

extern tsgl_display display;
extern tsgl_framebuffer framebuffer;

extern tsgl_pos width;
extern tsgl_pos height;
extern tsgl_colormode colormode;

extern tsgl_rawcolor white;
extern tsgl_rawcolor red;
extern tsgl_rawcolor orange;
extern tsgl_rawcolor green;
extern tsgl_rawcolor yellow;
extern tsgl_rawcolor blue;
extern tsgl_rawcolor magenta;
extern tsgl_rawcolor transparent;
extern tsgl_rawcolor black;
