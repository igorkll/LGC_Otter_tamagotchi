#pragma once
#include "../main.h"

#define ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT 5

typedef enum {
    game_room_bedroom = 0,
    game_room_kitchen
} game_room;
#define game_room uint8_t

typedef enum {
    game_person_otter = 0
} game_person;
#define game_person uint8_t

typedef struct {
    game_room room;
    game_person person;
} Game_state;

typedef struct {
    const char* background;
    const char* music;
    tsgl_pos person_x;
    tsgl_pos person_y;
} Room;

extern Game_state current_state;

void game_selectRoom(int index);
const Room* game_getCurrentRoom();
bool game_isLockedInRoom();
const char* game_getCurrentPerson();
void game_start();
