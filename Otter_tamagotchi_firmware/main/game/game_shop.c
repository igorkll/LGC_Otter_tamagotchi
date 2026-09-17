#include "game_shop.h"

#define SHOPOVERLAY_TEXT_TARGET_WIDTH 8
#define SHOPOVERLAY_TEXT_TARGET_HEIGHT 8

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

void shop_draw_overlay() {
    printsettings.fg = green;

    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "Balance: %i\n", current_state.states_money);

    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y, printsettings, text);
}

bool shop_buy(int* countvar, int price) {
    if (price > current_state.states_money) return false;
    (*countvar)++; //что тут не так. я хочу инкрементить значения которое находится по указателю
    current_state.states_money -= price;
    return true;
}
