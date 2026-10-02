#include "game_slotmachine.h"
#include "../gfx.h"
#include "../hctl.h"

#define SLOTMACHINE_MIN_MONEY 10
#define SLOTMACHINE_MAX_MONEY 1000
#define SLOTMACHINE_MONEY_STEP 10

#define SLOTMACHINE_Y_CENTER_OFFSET 10
#define SLOTMACHINE_TEXT_TARGET_WIDTH 8
#define SLOTMACHINE_TEXT_TARGET_HEIGHT 16

tsgl_print_settings printsettings_slotmachine = {
    .locationMode = tsgl_print_start_top,

    // multiline
    .multiline = true,
    .globalCentering = true,
    .alignment = tsgl_print_alignment_center,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = SLOTMACHINE_TEXT_TARGET_WIDTH,
    .targetHeight = SLOTMACHINE_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,

    .stroke_thickness = 1,
    .stroke_no_clamp = true
};

static void _run() {

}

void game_slotmachine_draw() {
    if (!current_state.overlay_slotmachine) return;

    // ------------------- process
    if (tsgl_keyboard_whenPressedOrHolded(&keyboard, KEY_INDEX_LEFT)) {
        current_state.slotmachine_money -= SLOTMACHINE_MONEY_STEP;
        if (current_state.slotmachine_money < SLOTMACHINE_MIN_MONEY) current_state.slotmachine_money = SLOTMACHINE_MAX_MONEY;
    }

    if (tsgl_keyboard_whenPressedOrHolded(&keyboard, KEY_INDEX_OKAY)) {
        _run();
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_RIGHT)) {
        current_state.slotmachine_money += SLOTMACHINE_MONEY_STEP;
        if (current_state.slotmachine_money > SLOTMACHINE_MAX_MONEY) current_state.slotmachine_money = SLOTMACHINE_MIN_MONEY;
    }

    // ------------------- draw
    gfx_drawCenteredImage(WIDTH / 2, HEIGHT / 2, "/firmware/images/slotmchn.bmp");

    printsettings_slotmachine.fg = current_state.slotmachine_money > current_state.states_money ? red : green;

    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xC4\xE5\xEF\xED\xF3\xF2\xFC: %i", current_state.slotmachine_money); //Депнуть
    printsettings_slotmachine.width = WIDTH;
    printsettings_slotmachine.height = SLOTMACHINE_TEXT_TARGET_HEIGHT;
    printsettings_slotmachine.stroke = black;
    tsgl_framebuffer_text(&framebuffer, 0, (HEIGHT / 2) + SLOTMACHINE_Y_CENTER_OFFSET, printsettings_slotmachine, text);
}

void game_slotmachine_open() {
    if (current_state.overlay_slotmachine) return;
    current_state.overlay_slotmachine = true;
    
}

void game_slotmachine_close() {
    if (!current_state.overlay_slotmachine) return;
    current_state.overlay_slotmachine = false;

}
