#include "game_room_overlays.h"
#include "game_shop.h"
#include "game_upmenu.h"
#include "game_printsets.h"

void kitchen_draw_overlay() {
    printsettings_overlay.fg = green;
    printsettings_overlay.stroke = black;

    int itemNum = game_upmenu_currentSelected() - L2;
    int* countPtr = shop_getItemPtr(itemNum);
    if (countPtr == NULL) return;
    int count = *countPtr;
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xCE\xF1\xF2\xE0\xEB\xEE\xF1\xFC: %i\n", count); //Осталось
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y, printsettings_overlay, text);
}

void game_roomOverlay() {
    switch (game_getCurrentRoomIndex()) {
        case ID_KITCHEN:
            kitchen_draw_overlay();
            break;
        
        case ID_SHOP:
            shop_draw_overlay();
            break;
    }
}