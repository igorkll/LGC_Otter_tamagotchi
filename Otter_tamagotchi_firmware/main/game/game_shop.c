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

static int getSelectedItemPrice() {

}

void shop_draw_overlay() {
    printsettings.fg = green;
    printsettings.stroke = black;

    char text[MAX_ACTION_LEN];

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xC1\xE0\xEB\xE0\xED\xF1: %i\n", current_state.states_money); //Баланс
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y, printsettings, text);

    int price = getSelectedItemPrice();
    if (price < 0) return;
    
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xD6\xE5\xED\xE0: %i\n", price); //Цена
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y + SHOPOVERLAY_TEXT_OFFSET, printsettings, text);
}

bool shop_buy(int* countvar, int price) {
    if (price > current_state.states_money) return false;
    (*countvar)++; //что тут не так. я хочу инкрементить значения которое находится по указателю
    current_state.states_money -= price;
    return true;
}
