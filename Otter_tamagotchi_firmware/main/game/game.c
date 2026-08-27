#include "game.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../pushsound.h"
#include "game_upmenu.h"

// ------------------------------------ consts

static const char* game_state_path = "/storage/game_state";
static const char* game_rooms_images_paths[] = {
    "/storage/rooms/bedroom.bmp",
    "/storage/rooms/kitchen.bmp"
};

static const Game_state default_state = {
    .room = 0
};

// ------------------------------------ vars

Game_state current_state;
static tsgl_benchmark benchmark;
static tsgl_sprite* room_sprite = NULL;

// ------------------------------------ functions

static void loadSprites() {
    static int room_sprite_old_index = 0;
    if (room_sprite == NULL || current_state.room != room_sprite_old_index) {
        room_sprite_old_index = current_state.room;
        if (room_sprite != NULL) tsgl_bmp_free(room_sprite);
        room_sprite = gfx_loadSprite(game_rooms_images_paths[current_state.room]);
        ESP_LOGI(TAG, "room loaded");
    }
}

static void game_save() {
    tsgl_filesystem_writeFile(game_state_path, &current_state, sizeof(Game_state));
}

static void game_load() {
    if (tsgl_filesystem_exists(game_state_path)) {
        tsgl_filesystem_readFile(game_state_path, &current_state, sizeof(Game_state));
    } else {
        memcpy(&current_state, &default_state, sizeof(Game_state));
    }
}

void game_start() {
    ESP_LOGI(TAG, "game started!");
    tsgl_benchmark_reset(&benchmark);
    game_load();

    pushsound_play("/storage/sounds/question.pcm", 16000);
    tsgl_delay(3000);

    pushsound_play("/storage/sounds/sadness.pcm", 16000);
    tsgl_delay(3000);

    pushsound_play("/storage/sounds/trigger.pcm", 16000);
    tsgl_delay(3000);

    tsgl_sound* sound = pushsound_play("/storage/test.pcm", 8000);

    while (true) {
        hctl_process();
        game_upmenu_process();

        tsgl_benchmark_startRendering(&benchmark);
        loadSprites();
        //tsgl_framebuffer_clear(&framebuffer, black);
        gfx_drawCenteredScreenImageSprite(room_sprite);
        game_upmenu_draw();
        tsgl_benchmark_endRendering(&benchmark);

        tsgl_benchmark_startSend(&benchmark);
        tsgl_display_send(&display, &framebuffer);
        tsgl_benchmark_endSend(&benchmark);

        //tsgl_benchmark_print(&benchmark);
        tsgl_benchmark_wait(&benchmark, 15);
    }
}
