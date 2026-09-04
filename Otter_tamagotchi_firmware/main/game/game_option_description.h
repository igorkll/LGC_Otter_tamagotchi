#pragma once
#include "game.h"
#include "game_upmenu.h"

typedef struct {
    const char* arr[GAME_UPMENU_COUNT];
} Room_option_descriptions;

extern Room_option_descriptions game_rooms_option_descriptions[];
