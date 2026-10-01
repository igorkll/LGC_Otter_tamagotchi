#include "game_room_overlays.h"
#include "game_shop.h"
#include "game_upmenu.h"
#include "game_printsets.h"
#include "game_states.h"
#include "../gfx.h"

#define EMOTE_SIZE 26 
#define EMOTE_MARGIN 5
#define EMOTE_GAP 5

#define EMOTE_DRAW_START_X EMOTE_MARGIN
#define EMOTE_DRAW_Y (HEIGHT - ROOM_OVERLAY_Y_BASE - EMOTE_SIZE - EMOTE_MARGIN)

#define TEXT_TARGET_HEIGHT 9
#define TEXT_GAP 4
#define TEXT_OFFSET (TEXT_TARGET_HEIGHT + TEXT_GAP)

static float getCurrentValue(int item) {
    switch (item) {
        case 0:
            return current_state.states_hunger;

        case 1:
            return current_state.states_thirst;
    }

    return 0;
}

static float getRecoverValue(int item) {
    switch (item) {
        case 0:
            return EAT_RECOVER;

        case 1:
            return WATER_RECOVER;
    }

    return 0;
}

static void kitchen_draw_overlay() {
    printsettings_overlay.fg = green;
    printsettings_overlay.stroke = black;

    int itemNum = game_upmenu_currentSelected() - L2;
    int* countPtr = shop_getItemPtr(itemNum);
    if (countPtr == NULL) return;
    int count = *countPtr;

    float currentValue = getCurrentValue(itemNum);
    float recoverValue = getRecoverValue(itemNum);
    float needEat = 0;
    if (recoverValue > 0) {
        needEat = currentValue / recoverValue;
    }
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xCE\xF1\xF2\xE0\xEB\xEE\xF1\xFC: %i\n", count); //Осталось
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y, printsettings_overlay, text);

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "\xCD\xF3\xE6\xED\xEE: %.1f\n", needEat); //Нужно
    tsgl_framebuffer_text(&framebuffer, ROOM_OVERLAY_START_X, ROOM_OVERLAY_START_Y + TEXT_OFFSET, printsettings_overlay, text);
}

static void draw_emote(tsgl_pos* emote_x, const char* path) {
    gfx_imageWithTransparentSupport(*emote_x, EMOTE_DRAW_Y, path);
    *emote_x += EMOTE_SIZE + EMOTE_GAP;
}   

static void draw_emotions() {
    tsgl_pos emote_x = EMOTE_DRAW_START_X;

    if (game_states_is_fatigue_critical()) {
        draw_emote(&emote_x, "/firmware/emotes/fatigue.bmp");
    }

    if (game_states_is_hunger_critical()) {
        draw_emote(&emote_x, "/firmware/emotes/hunger.bmp");
    }

    if (game_states_is_thirst_critical()) {
        draw_emote(&emote_x, "/firmware/emotes/thirst.bmp");
    }

    if (game_states_is_caress_critical() || game_states_is_sadness_critical()) {
        draw_emote(&emote_x, "/firmware/emotes/patpat.bmp");
    }
}

void game_roomOverlay() {
    draw_emotions();

    switch (game_getCurrentRoomIndex()) {
        case game_room_kitchen:
            kitchen_draw_overlay();
            break;
        
        case game_room_shop:
            shop_draw_overlay();
            break;
    }
}