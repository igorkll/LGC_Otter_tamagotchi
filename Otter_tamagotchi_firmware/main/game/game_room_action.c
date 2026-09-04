#include "game_room_action.h"
#include "game.h"

// начало второй линии кнопок
#define L2 5

static void car_selectRoom(game_room moveTo) {
    if (current_state.old_car_room == moveTo) {
        game_selectRoom(moveTo);
    } else {
        current_state.old_car_room = -1;
        
        // Поехали!
        game_startActionTimer(GAMECFG_CAR_MOVE_TIME, "\xCF\xEE\xE5\xF5\xE0\xEB\xE8\x21", game_action_switchRoom, moveTo);
    }
}

void game_yard_roomAction(int action) {
    switch (action) {
        case L2:
            game_selectRoom(ID_CAR);
            break;
    }
}

void game_car_roomAction(int action) {
    switch (action) {
        case 0:
            car_selectRoom(ID_YARD);
            break;
        
        case 1:
            car_selectRoom(ID_SHOP);
            break;
    }
}

void game_shop_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(ID_CAR);
            break;
    }
}

void game_roomAction(int action) {
    switch (game_getCurrentRoomIndex()) {
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
}
