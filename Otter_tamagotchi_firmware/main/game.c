#include "game.h"

static const char* game_state_path = "/storage/game_state";

static Game_state default_state = {
    .room = 0
};

Game_state current_state;

static void game_save() {
    tsgl_filesystem_writeFile(game_state_path, &current_state, sizeof(Game_state))
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
    game_load();

    while (true) {
        tsgl_framebuffer_clear(&framebuffer, white);
        tsgl_display_send(&display, &framebuffer);
        tsgl_delay(10);
    }
}
