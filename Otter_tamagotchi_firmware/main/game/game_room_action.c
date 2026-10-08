#include "game_room_action.h"
#include "game_actions.h"
#include "game_upmenu.h"
#include "game.h"
#include "game_backpack.h"
#include "game_states.h"
#include "game_combinemenu.h"
#include "game_shop.h"
#include "game_slotmachine.h"
#include "../pushsound.h"

#include "../subgames/racing.h"
#include "../subgames/tetris.h"
#include "../subgames/island.h"
#include "../subgames/snake.h"

static tsgl_sound* room_music;

static void stopRoomMusic() {
    if (room_music) {
        tsgl_sound_free(room_music);
        room_music = NULL;
    }
}

static void startRoomMusic(int musicSampleRate, const char* str, float volume) {
    stopRoomMusic();

    char path[MAX_PATH_LEN];
    TSGL_funcs_slnprintf(path, MAX_PATH_LEN, "/firmware/music/%s.dpw", str);
    room_music = pushsound_loop(path, musicSampleRate, volume);
    tsgl_sound_setLoop(room_music, false); // запускаю музыку как loop а потом выключаю loop. чтобы она была привязана к переменной громкости музыки
}

static void car_roomSelect(game_room room) {
    current_state.next_car_icon = -1;
    
    switch (room) {
        case game_room_yard:
            current_state.next_car_icon = 0;
            break;

        case game_room_shop:
            current_state.next_car_icon = 1;
            break;

        case game_room_club:
            current_state.next_car_icon = 2;
            break;

        case game_room_museum:
            current_state.next_car_icon = 3;
            break;
    }

    game_updateActiveIcons();
}

static void car_selectRoom(game_room moveTo) {
    if (current_state.old_car_room == moveTo) {
        game_selectRoom(moveTo);
    } else if (current_state.actionTimer <= 0 || moveTo != current_state.actionTimer_nextRoom) {
        current_state.old_car_room = -1;

        // Поехали!
        game_startActionTimer(GAMECFG_CAR_MOVE_TIME, "\xCF\xEE\xE5\xF5\xE0\xEB\xE8\x21", game_action_switchRoom, moveTo, true);
        car_roomSelect(moveTo);
    }
}

static void game_bedroom_roomAction(int action) {
    switch (action) {
        case L2:
            game_actions_sleep(GAMECFG_FULL_SLEEP_TIME);
            break;

        case L2 + 1:
            game_actions_patPat();
            break;
    }
}

static void game_kitchen_roomAction(int action) {
    switch (action) {
        case L2:
            game_actions_eat();
            break;

        case L2 + 1:
            game_actions_drink();
            break;
    }
}

static bool checkGameAllowed() {
    if (game_states_is_fatigue_critical()) {
        game_alt_message = "\xDF\x20\xF3\xF1\xF2\xE0\xEB\x2E\x2E\x2E\n\xD5\xEE\xF7\xF3\x20\xF1\xEF\xE0\xF2\xFC"; //Я устал...\nХочу спать
        return false;
    }

    if (game_states_is_hunger_critical()) {
        game_alt_message = "\xDF\x20\xE3\xEE\xEB\xEE\xE4\xED\xFB\xE9\n\x3A\x28"; //Я голодный\n:(
        return false;
    }

    if (game_states_is_thirst_critical()) {
        game_alt_message = "\xDF\x20\xF5\xEE\xF7\xF3\x20\xEF\xE8\xF2\xFC\x5C\x6E\x3A\x28"; //Я хочу пить\n:(
        return false;
    }

    if (game_states_is_sadness_critical()) { //печаль
        game_alt_message = "\xCC\xFF\xFF\x2E\x2E\x2E\n\xC0\x20\xEF\xEE\xE3\xEB\xE0\xE4\xE8\xF2\xFC\x3F\n\x3A\x28"; //Мяя...\nА погладить?\n:(
        return false;
    }

    if (game_states_is_caress_critical()) { //нежность
        game_alt_message = "\xD1\xED\xE0\xF7\xE0\xEB\xE0\x20\xEF\xEE\xE3\xEB\xE0\xE4\xE8\xF2\xFC\n\x3A\x29"; //Сначала погладить\n:)
        return false;
    }

    return true;
}

static void game_gaming_roomAction(int action) {
    switch (action) {
        case L2: {
            if (!checkGameAllowed()) return;
            subgame_racing_start();
            game_alt_handle = subgame_racing_handle;
            game_alt_exit = subgame_racing_exit;
            break;
        }

        case L2 + 1: {
            if (!checkGameAllowed()) return;
            subgame_tetris_start();
            game_alt_handle = subgame_tetris_handle;
            game_alt_exit = subgame_tetris_exit;
            break;
        }

        case L2 + 2: {
            if (!checkGameAllowed()) return;
            subgame_island_start();
            game_alt_handle = subgame_island_handle;
            game_alt_exit = subgame_island_exit;
            break;
        }

        case L2 + 3: {
            if (!checkGameAllowed()) return;
            subgame_snake_start();
            game_alt_handle = subgame_snake_handle;
            game_alt_exit = subgame_snake_exit;
            break;
        }
    }
}

static void game_yard_roomAction(int action) {
    switch (action) {
        case L2:
            game_selectRoom(game_room_car);
            break;
    }
}

static void game_car_roomAction(int action) {
    switch (action) {
        case 0:
            car_selectRoom(game_room_yard);
            break;
        
        case 1:
            car_selectRoom(game_room_shop);
            break;

        case 2:
            car_selectRoom(game_room_club);
            break;

        case 3:
            car_selectRoom(game_room_museum);
            break;
    }
}

static void game_shop_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(game_room_car);
            break;

        case L2:
            shop_buy(shop_getItemPtr(0), shop_getItemPrice(0));
            break;

        case L2 + 1:
            shop_buy(shop_getItemPtr(1), shop_getItemPrice(1));
            break;
    }
}

static void game_club_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(game_room_car);
            break;

        case L2:
            game_selectRoom(game_room_fear);
            break;
    }
}

static void game_museum_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(game_room_car);
            break;

        case L2:
            startRoomMusic(SOUND_DFPWM_SAMPLERATE, "gmp0", 1);
            break;

        case L2 + 1:
            startRoomMusic(SOUND_DFPWM_SAMPLERATE, "gmp1", 1);
            break;
    }
}

static void game_fear_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(game_room_club);
            break;

        case L2:
            game_slotmachine_open();
            break;
    }
}

void game_roomAction(int action) {
    switch (game_getCurrentRoomIndex()) {
        case game_room_bedroom:
            game_bedroom_roomAction(action);
            break;

        case game_room_kitchen:
            game_kitchen_roomAction(action);
            break;

        case game_room_gaming:
            game_gaming_roomAction(action);
            break;

        case game_room_yard:
            game_yard_roomAction(action);
            break;

        case game_room_car:
            game_car_roomAction(action);
            break;

        case game_room_shop:
            game_shop_roomAction(action);
            break;

        case game_room_club:
            game_club_roomAction(action);
            break;

        case game_room_museum:
            game_museum_roomAction(action);
            break;

        case game_room_fear:
            game_fear_roomAction(action);
            break;
    }

    switch (action) {
        case ID_CONSTIEM_OVERLAY:
            game_combinemenu_toggle();
            break;
    }
}

void game_roomSelected(game_room selected) {
    if (selected != game_room_car) current_state.old_car_room = selected;

    switch (selected) {
        case game_room_car:
            car_roomSelect(current_state.old_car_room);
            break;
    }
}

void game_stopGameActionRoomMusic() {
    stopRoomMusic();
}
