#include "game.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../pushsound.h"
#include "../funcs.h"
#include "game_upmenu.h"

// ------------------------------------ consts

static const char* game_state_path = "/storage/game_state";
static const char* game_rooms_images[] = {
    "bedroom",
    "kitchen"
};

#define ROOMS_COUNT (sizeof(game_rooms_images) / sizeof(&game_rooms_images))

static const Game_state default_state = {
    .room = 0
};

// ------------------------------------ vars

Game_state current_state;
Game_state old_state;
static tsgl_benchmark benchmark;
static tsgl_sprite* room_sprite = NULL;

// ------------------------------------ functions

const char* game_getCurrentRoom() {
    return game_rooms_images[current_state.room];
}

static void loadSprites() {
    static int room_sprite_old_index = 0;
    if (room_sprite == NULL || current_state.room != room_sprite_old_index) {
        room_sprite_old_index = current_state.room;
        if (room_sprite != NULL) tsgl_bmp_free(room_sprite);

        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/storage/rooms/%s.bmp", game_getCurrentRoom());
        room_sprite = gfx_loadSprite(path);
    }
}

static void game_save() {
    tsgl_filesystem_writeFile(game_state_path, &current_state, sizeof(Game_state));
    ESP_LOGI(TAG, "game saved!");
}

static void game_load() {
    if (tsgl_filesystem_exists(game_state_path)) {
        tsgl_filesystem_readFile(game_state_path, &current_state, sizeof(Game_state));
    } else {
        memcpy(&current_state, &default_state, sizeof(Game_state));
    }
    ESP_LOGI(TAG, "game loaded!");
}

// ------------------------------------ process

static void selectRoom(int index) {
    current_state.room = index;
    game_upmenu_setActivate(0, index == 0);
    game_upmenu_setActivate(1, index == 1);
}

static void start() {
    selectRoom(current_state.room);
}

static void exit_myaaaa(tsgl_sound* sound) {
    bool* running = (bool*)sound->userData;
    *running = false;
}

static void run_myaaaa() {
    hctl_resetIdleTimer();
    hctl_setBacklight(BACKLIGHT_MAX);

    gfx_drawCenteredScreenImage("/storage/myaaaa/myaaaa.bmp");
    tsgl_display_send(&display, &framebuffer);

    bool running = true;

    tsgl_sound* sound = pushsound_play("/storage/myaaaa/myaaaa.pcm", 8000);
    tsgl_sound_setVolume(sound, 6);
    sound->userData = (void*)&running;
    tsgl_sound_attachCallback_end(sound, exit_myaaaa);

    while (running) {
        tsgl_keyboard_readAll(&keyboard);

        if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_CANCEL)) break;
    }
    
    tsgl_benchmark_reset(&benchmark);
}

static void process() {
    hctl_process();
    
    int used = game_upmenu_process();
    if (used >= 0 && used < ROOMS_COUNT) {
        selectRoom(used);
        if (used == 1) {
            run_myaaaa();
        }
    }

    if (memcmp(&current_state, &old_state, sizeof(Game_state)) != 0) {
        old_state = current_state;
        game_save();
    }
}

void game_start() {
    ESP_LOGI(TAG, "game started!");
    tsgl_benchmark_reset(&benchmark);
    game_load();

    game_upmenu_init();
    start();

    while (true) {
        process();

        tsgl_benchmark_startRendering(&benchmark);
        loadSprites();
        //tsgl_framebuffer_clear(&framebuffer, black);
        gfx_drawCenteredScreenImageSprite(room_sprite);
        game_upmenu_draw();
        tsgl_benchmark_endRendering(&benchmark);

        tsgl_benchmark_startSend(&benchmark);
        tsgl_display_send(&display, &framebuffer);
        tsgl_benchmark_endSend(&benchmark);

        tsgl_benchmark_print(&benchmark);
        tsgl_benchmark_wait(&benchmark, 15);
    }
}
