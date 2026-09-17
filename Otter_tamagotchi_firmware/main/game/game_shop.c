#include "shop.h"

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = BACKPACK_ICON_TEXT_WIDTH,
    .targetHeight = BACKPACK_ICON_TEXT_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,
    .stroke_thickness = 1,
    .stroke_no_clamp = true
};

void shop_draw_overlay() {

}

bool shop_buy(int* countvar, int price) {
    if (price > current_state.states_money) return false;
    *countvar++;
    current_state.states_money -= price;
    return true;
}
