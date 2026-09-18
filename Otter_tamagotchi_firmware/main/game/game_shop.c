#include "game_shop.h"
#include "game_upmenu.h"
#include "game_printsets.h"
#include "../pushsound.h"

#define SHOPOVERLAY_TEXT_TARGET_WIDTH 9
#define SHOPOVERLAY_TEXT_TARGET_HEIGHT 9
#define SHOPOVERLAY_TEXT_GAP 4
#define SHOPOVERLAY_TEXT_OFFSET (SHOPOVERLAY_TEXT_TARGET_HEIGHT + SHOPOVERLAY_TEXT_GAP)

int shop_getCurrentItem() {
    switch (game_upmenu_currentSelected()) {
        case L2:
            return 0;

        case L2 + 1:
            return 1;
    }

    return -1;
}

int shop_getItemPrice(int itemNum) {
    switch (itemNum) {
        case 0:
            return 15;

        case 1:
            return 8;
    }

    return -1;
}

int* shop_getItemPtr(int itemNum) {
    switch (itemNum) {
        case 0:
            return &current_state.backpack_eat_count;

        case 1:
            return &current_state.backpack_water_count;
    }

    return NULL;
}

void shop_draw_overlay() {
    printsettings_overlay.fg = green;
    printsettings_overlay.stroke = black;

    char text[MAX_ACTION_LEN];

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xC1\xE0\xEB\xE0\xED\xF1: %i\n", current_state.states_money); //Баланс
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y, printsettings_overlay, text);

    int currentItem = shop_getCurrentItem();
    int price = shop_getItemPrice(currentItem);
    int* itemPtr = shop_getItemPtr(currentItem);
    if (price < 0 || itemPtr == NULL) return;
    
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xD6\xE5\xED\xE0: %i\n", price); //Цена
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y + SHOPOVERLAY_TEXT_OFFSET, printsettings_overlay, text);

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xD3\x20\xE2\xE0\xF1: %i\n", *itemPtr); //У вас
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y + (SHOPOVERLAY_TEXT_OFFSET * 2), printsettings_overlay, text);
}

bool shop_buy(int* countvar, int price) {
    if (price > current_state.states_money) return false;
    pushsound_play("/firmware/sounds/buy.pcm", 16000, EFFECTS_SOUND_VOLUME * BUY_SOUND_VOLUME);
    (*countvar)++;
    current_state.states_money -= price;
    return true;
}
