#include "game_room_overlays.h"
#include "game_shop.h"

void game_roomOverlay() {
    switch (game_getCurrentRoomIndex()) {
        case ID_SHOP:
            shop_draw_overlay();
            break;
    }
}