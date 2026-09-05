#pragma once
#include "../main.h"

#define ANYSTATUS_SIZE_MUL 0.8
#define ANYSTATUS_LINE_PADDING 4 
#define ANYSTATUS_HEIGHT 20
#define ANYSTATUS_OBJ_OFFSET 12

#define ANYSTATUS_TEXT_TARGET_WIDTH 10
#define ANYSTATUS_TEXT_TARGET_HEIGHT 16

void game_funcs_drawAnyStatus(tsgl_pos y, int state, int stateMin, int stateMax, const char* text, bool vflip);
