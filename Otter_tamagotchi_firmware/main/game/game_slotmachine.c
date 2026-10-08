#include "game_slotmachine.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../pushsound.h"

#define SLOTMACHINE_MIN_MONEY 10
#define SLOTMACHINE_MAX_MONEY 1000
#define SLOTMACHINE_MONEY_STEP 10

#define SLOTMACHINE_Y_CENTER_OFFSET 10
#define SLOTMACHINE_TEXT_TARGET_WIDTH 8
#define SLOTMACHINE_TEXT_TARGET_HEIGHT 16

#define SLOTMACHINE_BASE_NUM_POS_X -37
#define SLOTMACHINE_BASE_NUM_POS_Y -23
#define SLOTMACHINE_BASE_NUM_STEP_X 21

#define SLOTMACHINE_CURRENT_MONEY_X 18
#define SLOTMACHINE_CURRENT_MONEY_Y -28

#define SLOTMACHINE_MIN_SLOT 2
#define SLOTMACHINE_MAX_SLOT 9

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
    if (current_state.slotmachine_money > current_state.states_money) {
        return;
    }

    current_state.states_money -= current_state.slotmachine_money;

    bool win = true;
    int firstNum = -1;
    for (size_t i = 0; i < SLOTMACHINE_NUMS_COUNT; i++) {
        int num = tsgl_random(SLOTMACHINE_MIN_SLOT, SLOTMACHINE_MAX_SLOT);
        current_state.slotmachine_nums[i] = num;

        if (i == 0) {
            firstNum = num;
        } else if (num != firstNum) {
            win = false;
        }
    }

    if (win) {
        current_state.states_money += current_state.slotmachine_money * firstNum;
    }

    pushsound_play("/firmware/sounds/lever2.pcm", SOUND_EFFECTS_SAMPLERATE, LEVER2_SOUND_VOLUME);
}

tsgl_rawcolor _get_num_color(int num) {
    return tsgl_color_raw(tsgl_color_hsv(num * (255 / 9), 255, 255), framebuffer.colormode);
}

void game_slotmachine_draw() {
    if (!current_state.overlay_slotmachine) return;

    // ------------------- process
    if (tsgl_keyboard_whenPressedOrHoldWithTrigger(&keyboard, KEY_INDEX_LEFT)) {
        current_state.slotmachine_money -= SLOTMACHINE_MONEY_STEP;
        if (current_state.slotmachine_money < SLOTMACHINE_MIN_MONEY) current_state.slotmachine_money = SLOTMACHINE_MAX_MONEY;
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_OKAY)) {
        _run();
    }

    if (tsgl_keyboard_whenPressedOrHoldWithTrigger(&keyboard, KEY_INDEX_RIGHT)) {
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

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "%i", current_state.states_money);
    printsettings_slotmachine.width = SLOTMACHINE_TEXT_TARGET_WIDTH * 4;
    printsettings_slotmachine.height = SLOTMACHINE_TEXT_TARGET_HEIGHT;
    printsettings_slotmachine.stroke = black;
    printsettings_slotmachine.fg = green;
    tsgl_framebuffer_text(&framebuffer, (WIDTH / 2) + SLOTMACHINE_CURRENT_MONEY_X, (HEIGHT / 2) + SLOTMACHINE_CURRENT_MONEY_Y, printsettings_slotmachine, text);

    for (size_t i = 0; i < SLOTMACHINE_NUMS_COUNT; i++) {
        int num = current_state.slotmachine_nums[i];
        TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "%i", num);
        printsettings_slotmachine.width = SLOTMACHINE_TEXT_TARGET_WIDTH;
        printsettings_slotmachine.height = SLOTMACHINE_TEXT_TARGET_HEIGHT;
        printsettings_slotmachine.stroke = black;
        printsettings_slotmachine.fg = _get_num_color(num);
        tsgl_framebuffer_text(&framebuffer, (WIDTH / 2) + SLOTMACHINE_BASE_NUM_POS_X + (i * SLOTMACHINE_BASE_NUM_STEP_X), (HEIGHT / 2) + SLOTMACHINE_BASE_NUM_POS_Y, printsettings_slotmachine, text);
    }
}

void game_slotmachine_open() {
    if (current_state.overlay_slotmachine) return;
    current_state.overlay_slotmachine = true;
    
    if (current_state.slotmachine_nums[0] == 0) {
        for (size_t i = 0; i < SLOTMACHINE_NUMS_COUNT; i++) {
            int num = tsgl_random(SLOTMACHINE_MIN_SLOT, SLOTMACHINE_MAX_SLOT);
            current_state.slotmachine_nums[i] = num;
        }
    }

    pushsound_play("/firmware/sounds/lever1.pcm", SOUND_EFFECTS_SAMPLERATE, LEVER1_SOUND_VOLUME);
}

void game_slotmachine_close() {
    if (!current_state.overlay_slotmachine) return;
    current_state.overlay_slotmachine = false;
    pushsound_play("/firmware/sounds/lever0.pcm", SOUND_EFFECTS_SAMPLERATE, LEVER0_SOUND_VOLUME);
}
