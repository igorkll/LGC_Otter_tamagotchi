#pragma once
#include "../main.h"

typedef enum {
    game_room_bedroom = 0,
    game_room_kitchen
} game_room;
#define game_room uint8_t

typedef struct {
    game_room room;
} Game_state;

extern Game_state current_state;

const char* game_getCurrentRoom();
void game_start();
