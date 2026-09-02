#include "game_room_action.h"
#include "game.h"

// начало второй линии кнопок
#define L2 5

#define ID_YARD 4
#define ID_CAR 5
#define ID_SHOP 6

void game_yard_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(ID_CAR);
            break;
    }
}

void game_car_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(ID_YARD);
            break;
        
        case 1:
            game_selectRoom(ID_SHOP);
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
