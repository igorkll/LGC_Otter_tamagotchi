#include "game.h"
#include "gfx.h"
#include "hctl.h"

// ------------------------------------ consts

#define MAX_SOUNDS 16
#define SOUND_BUFFER_SIZE (16 * 1024)

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
        printf("load room\n");
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

static void play_sound(const char* path) {
    static tsgl_sound sounds[MAX_SOUNDS] = {0};
    static uint8_t current_sound_index = 0;

    tsgl_sound* current_sound = &sounds[current_sound_index];
    if (tsgl_sound_load_pcmEx(current_sound, SOUND_BUFFER_SIZE, 0, path, 8000, 1, 1, tsgl_sound_pcm_unsigned, true) != ESP_OK) {
        return;
    }
    
    current_sound_index++;
    if (current_sound_index >= MAX_SOUNDS) {
        current_sound_index = 0;
    }

    tsgl_sound_output* sound_outputs[] = {sound_output};
    tsgl_sound_attachCallback_end(current_sound, tsgl_sound_free);
    tsgl_sound_setOutputs(current_sound, sound_outputs, 1, false);
    tsgl_sound_setVolume(current_sound, 1);
    tsgl_sound_play(current_sound);
}

void game_start() {
    ESP_LOGI(TAG, "game started!");
    tsgl_benchmark_reset(&benchmark);
    game_load();

    play_sound("/storage/test.pcm");

    while (true) {
        hctl_process();

        tsgl_benchmark_startRendering(&benchmark);
        loadSprites();
        //tsgl_framebuffer_clear(&framebuffer, black);
        gfx_drawCenteredScreenImageSprite(room_sprite);
        tsgl_benchmark_endRendering(&benchmark);

        tsgl_benchmark_startSend(&benchmark);
        tsgl_display_send(&display, &framebuffer);
        tsgl_benchmark_endSend(&benchmark);

        tsgl_benchmark_print(&benchmark);
        tsgl_benchmark_wait(&benchmark, 15);
    }
}
