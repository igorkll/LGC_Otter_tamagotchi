#include "game.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../pushsound.h"
#include "../funcs.h"
#include "game_upmenu.h"
#include "game_room_action.h"

// ------------------------------------ consts

static const char* game_state_path = "/storage/gamestat";

const Room rooms[] = {
    {
        .background = "bedroom",
        .music = NULL,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "kitchen",
        .music = NULL,
        .person_x = (WIDTH / 4) - 20,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "gaming",
        .music = NULL,
        .person_x = (WIDTH / 4) + 20,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "toilet",
        .music = NULL,
        .person_x = (WIDTH / 4) * 2,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "yard",
        .music = NULL,
        .person_x = WIDTH / 4,
        .person_y = (HEIGHT / 4) * 2,
    },
    {
        .background = "car",
        .music = "car",
        .musicVolume = 1,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "shop",
        .music = "shop",
        .musicVolume = 0.6,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    }
};

static const char* game_persons_images[] = {
    "otter"
};

#define ROOMS_COUNT TSGL_CALC_ARRSIZE(rooms)

static const Game_state default_state = {
    .room = game_room_bedroom,
    .person = game_person_otter
};

// ------------------------------------ vars

Game_state current_state;
Game_state old_state;
static tsgl_benchmark benchmark;
static tsgl_sprite* room_sprite = NULL;
static tsgl_sprite* person_sprite = NULL;
static tsgl_sound* room_music;

// ------------------------------------ functions

const Room* game_getCurrentRoom() {
    return &rooms[current_state.room];
}

size_t game_getCurrentRoomIndex() {
    return current_state.room;
}

bool game_isLockedInRoom() {
    return current_state.room >= ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT;
}

const char* game_getCurrentPerson() {
    return game_persons_images[current_state.person];
}

void game_save() {
    if (tsgl_filesystem_writeFile(game_state_path, &current_state, sizeof(Game_state)) == sizeof(Game_state)) {
        ESP_LOGI(TAG, "game saved");
    } else {
        ESP_LOGE(TAG, "failed to save game");
    }
}

static void game_load() {
    memcpy(&current_state, &default_state, sizeof(Game_state));
    if (tsgl_filesystem_exists(game_state_path)) {
        if (tsgl_filesystem_readFile(game_state_path, &current_state, sizeof(Game_state)) == sizeof(Game_state)) {
            ESP_LOGI(TAG, "game loaded");
        } else {
            ESP_LOGE(TAG, "failed to load game");
        }
    } else {
        ESP_LOGI(TAG, "game default loaded");
    }
    memcpy(&old_state, &current_state, sizeof(Game_state));
}

static void unload_room_sound() {
    if (room_music) {
        tsgl_sound_free(room_music);
        room_music = NULL;
    }
}

void game_selectRoom(int index) {
    current_state.room = index;
    for (size_t i = 0; i < ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT; i++) {
        game_upmenu_setActivate(i, index == i);
    }

    unload_room_sound();

    const Room* room = game_getCurrentRoom();
    if (room->music != NULL) {
        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/firmware/music/%s.pcm", room->music);

        room_music = pushsound_loop(path, 4000, room->musicVolume);
        tsgl_sound_setLoop(room_music, true);
    }

    game_upmenu_reloadIcons();
}

// ------------------------------------ process

static void loadSprites() {
    static int room_sprite_old_index = 0;
    if (room_sprite == NULL || current_state.room != room_sprite_old_index) {
        room_sprite_old_index = current_state.room;
        if (room_sprite != NULL) tsgl_bmp_free(room_sprite);

        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/firmware/rooms/%s.bmp", game_getCurrentRoom()->background);
        room_sprite = gfx_loadSprite(path);
    }

    static int room_person_old_index = 0;
    if (person_sprite == NULL || current_state.person != room_person_old_index) {
        room_person_old_index = current_state.person;
        if (person_sprite != NULL) tsgl_bmp_free(person_sprite);

        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/firmware/persons/%s.bmp", game_getCurrentPerson());
        person_sprite = gfx_loadSprite(path);
    }
}

static void start() {
    game_selectRoom(current_state.room);
}

static void exit_myaaaa(tsgl_sound* sound) {
    bool* running = (bool*)sound->userData;
    *running = false;
}

static void run_myaaaa() {
    unload_room_sound();
    hctl_resetIdleTimer();
    hctl_setBacklight(BACKLIGHT_MAX);

    gfx_drawCenteredScreenImage("/firmware/myaaaa/myaaaa.bmp");
    tsgl_display_send(&display, &framebuffer);

    bool running = true;

    tsgl_sound* sound = pushsound_play("/firmware/myaaaa/myaaaa.pcm", 8000, MYAAAA_SOUND_VOLUME);
    sound->userData = (void*)&running;
    tsgl_sound_attachCallback_end(sound, exit_myaaaa);

    while (running) {
        tsgl_keyboard_readAll(&keyboard);

        if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_CANCEL)) {
            tsgl_sound_free(sound);
            running = false;
        }
    }
    
    tsgl_benchmark_reset(&benchmark);
    game_selectRoom(current_state.room);
}

static void process() {
    hctl_process();

    bool allPressed = true;
    for (size_t i = 0; i < KEYS_COUNT; i++) {
        if (!tsgl_keyboard_getState(&keyboard, i)) {
            allPressed = false;
            break;
        }
    }
    if (allPressed) {
        run_myaaaa();
    }
    
    int used = game_upmenu_process();
    if (used >= 0) {
        if (used < ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT && !game_isLockedInRoom()) {
            game_selectRoom(used);
        } else {
            game_roomAction(used);
        }
    }

    if (memcmp(&current_state, &old_state, sizeof(Game_state)) != 0) {
        old_state = current_state;
        game_save();
    }
}

static void drawPerson() {
    const Room* room = game_getCurrentRoom();
    gfx_drawCenteredImageSpriteWithTransparentSupport(room->person_x, room->person_y, person_sprite);
}

void game_start() {
    ESP_LOGI(TAG, "game started!");
    tsgl_benchmark_reset(&benchmark);
    game_load();

    game_upmenu_init();
    start();

    bool firstGameFrame = true;
    while (true) {
        process();

        tsgl_benchmark_startRendering(&benchmark);
        loadSprites();
        //tsgl_framebuffer_clear(&framebuffer, black);
        gfx_drawCenteredScreenImageSprite(room_sprite);
        drawPerson();
        game_upmenu_draw();
        tsgl_benchmark_endRendering(&benchmark);

        tsgl_benchmark_startSend(&benchmark);
        tsgl_display_send(&display, &framebuffer);
        tsgl_benchmark_endSend(&benchmark);

        #ifdef DEBUG_FPS
            tsgl_benchmark_print(&benchmark);
        #endif
        tsgl_benchmark_wait(&benchmark, TARGET_FPS);

        if (firstGameFrame) {
            hctl_setBacklightAndWait(BACKLIGHT_MAX);
            firstGameFrame = false;
        }
    }
}
