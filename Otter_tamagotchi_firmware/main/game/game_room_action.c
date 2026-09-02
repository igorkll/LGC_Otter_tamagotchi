#include "game_room_action.h"
#include "game.h"

void game_yard_roomAction(int action) {
    if (action == 0) {
        game_selectRoom(5);
    }
}

void game_car_roomAction(int action) {
    if (action == 0) {
        game_selectRoom(4);
    }
}


void game_roomAction(int action) {
    switch (game_getCurrentRoomIndex()) {
        case 4:
            game_yard_roomAction(action);
            break;

        case 5:
            game_car_roomAction(action);
            break;
        
        default:
            break;
    }
}
