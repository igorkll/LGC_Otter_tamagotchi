#pragma once
#include "../main.h"

#define ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT 5

#define ID_BEDROOM 0
#define ID_KITCHEN 1
#define ID_GAMING 2
#define ID_TOILET 3
#define ID_YARD 4
#define ID_CAR 5
#define ID_SHOP 6
#define ID_CLUB 7
#define ID_MUSEUM 8

#define ID_CONSTIEM_OVERLAY 9

#define RESET_SETTINGS_ID 8

typedef double game_state_val;

typedef enum {
    game_room_bedroom = 0,
    game_room_kitchen
} game_room;
#define game_room int8_t

typedef enum {
    game_person_otter = 0
} game_person;
#define game_person int8_t

typedef enum {
    game_action_switchRoom = 0
} game_action;
#define game_action int8_t

typedef struct {
    uint16_t resetSettingsId;

    // main
    game_room room;
    game_person person;
    
    // car
    game_room old_car_room;
    int8_t next_car_icon;

    // action timer
    int actionTimer;
    int actionTimer_max;
    char actionTimer_str[MAX_ACTION_LEN];
    game_action actionTimer_action;
    game_room actionTimer_nextRoom;
    bool actionTimer_allowCancel;

    // sleep
    int sleepTimer;
    int sleepStartTimer;

    // combine menu
    bool combinemenu_opened;
    int8_t combinemenu_index;

    // backpack
    bool backpack_opened;

    // states
    bool states_opened;
    int16_t states_money;
    game_state_val states_fatigue;
    game_state_val states_hunger;
    game_state_val states_thirst;
    game_state_val states_caress;
    game_state_val states_sadness;
} Game_state;

typedef struct {
    const char* background;
    const char* music;
    float musicVolume;
    uint16_t musicSampleRate;
    tsgl_pos person_x;
    tsgl_pos person_y;

    game_state_val states_delta_fatigue;
    game_state_val states_delta_hunger;
    game_state_val states_delta_thirst;
    game_state_val states_delta_caress;
    game_state_val states_delta_sadness;
} Room;

extern Game_state current_state;

void game_selectRoom(int index);
const Room* game_getCurrentRoom();
size_t game_getCurrentRoomIndex();
bool game_isLockedInRoom();
const char* game_getCurrentPerson();
void game_save();
void game_start();
void game_startActionTimer(int actionTimer, const char* str, game_action action, game_room nextRoom, bool allowCancel);
void game_stopActionTimer();
void game_sleepIn();
void game_updateActiveIcons();
bool game_isAnyOverlayOpened();
