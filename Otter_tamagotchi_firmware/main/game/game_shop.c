#include "game_shop.h"

#define SHOPOVERLAY_TEXT_TARGET_WIDTH 8
#define SHOPOVERLAY_TEXT_TARGET_HEIGHT 8
#define SHOPOVERLAY_TEXT_GAP 2
#define SHOPOVERLAY_TEXT_OFFSET (SHOPOVERLAY_TEXT_TARGET_HEIGHT + SHOPOVERLAY_TEXT_GAP)

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = SHOPOVERLAY_TEXT_TARGET_WIDTH,
    .targetHeight = SHOPOVERLAY_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,
    .stroke_thickness = 1,
    .stroke_no_clamp = true
};

int shop_getCurrentItem() {
    switch (action) {
        case L2:
            return 0;

        case L2 + 1:
            return 1;
    }

    return -1;
}

int shop_getItemPrice(int itemNum) {
    switch (action) {
        case 0:
            return 15;

        case 1:
            return 8;
    }

    return -1;
}

int* shop_getItemPtr(int itemNum) {
    switch (action) {
        case 0:
            return &current_state.backpack_eat_count;

        case 1:
            return &current_state.backpack_water_count;
    }

    return NULL;
}

void shop_draw_overlay() {
    printsettings.fg = green;
    printsettings.stroke = black;

    char text[MAX_ACTION_LEN];

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xC1\xE0\xEB\xE0\xED\xF1: %i\n", current_state.states_money); //Баланс
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y, printsettings, text);

    int currentItem = shop_getCurrentItem();
    int price = shop_getItemPrice(currentItem);
    int* itemPtr = shop_getItemPtr(currentItem);
    if (price < 0 || itemPtr == NULL) return;
    
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xD6\xE5\xED\xE0: %i\n", price); //Цена
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y + SHOPOVERLAY_TEXT_OFFSET, printsettings, text);

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xD3\x20\xE2\xE0\xF1: %i\n", *itemPtr); //У вас
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y + (SHOPOVERLAY_TEXT_OFFSET * 2), printsettings, text);
}

bool shop_buy(int* countvar, int price) {
    if (price > current_state.states_money) return false;
    (*countvar)++;
    current_state.states_money -= price;
    return true;
}
