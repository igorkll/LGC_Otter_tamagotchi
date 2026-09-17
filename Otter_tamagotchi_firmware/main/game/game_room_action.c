#include "game_room_action.h"
#include "game_actions.h"
#include "game_upmenu.h"
#include "game.h"
#include "game_backpack.h"
#include "game_states.h"
#include "game_combinemenu.h"
#include "game_shop.h"
#include "../pushsound.h"

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
        case ID_YARD:
            current_state.next_car_icon = 0;
            break;

        case ID_SHOP:
            current_state.next_car_icon = 1;
            break;

        case ID_CLUB:
            current_state.next_car_icon = 2;
            break;

        case ID_MUSEUM:
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

static void game_yard_roomAction(int action) {
    switch (action) {
        case L2:
            game_selectRoom(ID_CAR);
            break;
    }
}

static void game_car_roomAction(int action) {
    switch (action) {
        case 0:
            car_selectRoom(ID_YARD);
            break;
        
        case 1:
            car_selectRoom(ID_SHOP);
            break;

        case 2:
            car_selectRoom(ID_CLUB);
            break;

        case 3:
            car_selectRoom(ID_MUSEUM);
            break;
    }
}

static void game_shop_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(ID_CAR);
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
            game_selectRoom(ID_CAR);
            break;
    }
}

static void game_museum_roomAction(int action) {
    switch (action) {
        case 0:
            game_selectRoom(ID_CAR);
            break;

        case 5:
            startRoomMusic(16000, "gmp0", 1);
            break;

        case 6:
            startRoomMusic(16000, "gmp1", 1);
            break;
    }
}

void game_roomAction(int action) {
    switch (game_getCurrentRoomIndex()) {
        case ID_BEDROOM:
            game_bedroom_roomAction(action);
            break;

        case ID_KITCHEN:
            game_kitchen_roomAction(action);
            break;

        case ID_YARD:
            game_yard_roomAction(action);
            break;

        case ID_CAR:
            game_car_roomAction(action);
            break;

        case ID_SHOP:
            game_shop_roomAction(action);
            break;

        case ID_CLUB:
            game_club_roomAction(action);
            break;

        case ID_MUSEUM:
            game_museum_roomAction(action);
            break;
    }

    switch (action) {
        case ID_CONSTIEM_OVERLAY:
            game_combinemenu_toggle();
            break;
    }
}

void game_roomSelected(game_room selected) {
    if (selected != ID_CAR) current_state.old_car_room = selected;

    switch (selected) {
        case ID_CAR:
            car_roomSelect(current_state.old_car_room);
            break;
    }
}

void game_stopGameActionRoomMusic() {
    stopRoomMusic();
}
