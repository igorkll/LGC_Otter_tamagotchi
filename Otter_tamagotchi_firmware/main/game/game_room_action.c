#include "game_room_action.h"
#include "game_actions.h"
#include "game_upmenu.h"
#include "game.h"
#include "game_backpack.h"
#include "game_states.h"

// начало второй линии кнопок
#define L2 5

static void car_roomSelect(game_room room) {
    switch (room) {
        case ID_YARD:
            current_state.next_car_room = 0;
            break;

        case ID_SHOP:
            current_state.next_car_room = 1;
            break;
    }
}

static void car_selectRoom(game_room moveTo) {
    if (current_state.old_car_room == moveTo) {
        game_selectRoom(moveTo);
    } else if (current_state.actionTimer <= 0 || moveTo != current_state.actionTimer_nextRoom) {
        current_state.old_car_room = -1;

        // Поехали!
        game_startActionTimer(GAMECFG_CAR_MOVE_TIME, "\xCF\xEE\xE5\xF5\xE0\xEB\xE8\x21", game_action_switchRoom, moveTo, true);
        car_roomSelect(moveTo);
    }
}

static void game_bedroom_roomAction(int action) {
    switch (action) {
        case L2:
            game_actions_sleep(60);
            break;
    }
} 

static void game_yard_roomAction(int action) {
    switch (action) {
        case L2:
            game_selectRoom(ID_CAR);
            break;
    }
}

static void game_car_roomAction(int action) {
    switch (action) {
        case 0:
            car_selectRoom(ID_YARD);
            break;
        
        case 1:
            car_selectRoom(ID_SHOP);
            break;
    }
}

static void game_shop_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(ID_CAR);
            break;
    }
}

void game_roomAction(int action) {
    switch (game_getCurrentRoomIndex()) {
        case ID_BEDROOM:
            game_bedroom_roomAction(action);
            break;

        case ID_YARD:
            game_yard_roomAction(action);
            break;

        case ID_CAR:
            game_car_roomAction(action);
            break;

        case ID_SHOP:
            game_shop_roomAction(action);
            break;
    }

    switch (action) {
        case ID_CONSTIEM_STATES:
            game_states_toggle();
            break;
            
        case ID_CONSTIEM_BACKPACK:
            game_backpack_toggle();
            break;
    }
}

void game_roomSelected(game_room selected) {
    if (selected != ID_CAR) current_state.old_car_room = selected;

    switch (selected) {
        case ID_CAR:
            car_roomSelect(current_state.old_car_room);
            break;
    }
}
