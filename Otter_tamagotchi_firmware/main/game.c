#include "game.h"
#include "gfx.h"

static const char* game_state_path = "/storage/game_state";
static const char* game_rooms_images_paths[] = {
    "/storage/rooms/bedroom.bmp",
    "/storage/rooms/kitchen.bmp"
};

static Game_state default_state = {
    .room = 0
};

Game_state current_state;

static tsgl_sprite* room_sprite = NULL;
static int room_sprite_old_index = 0;

static void loadSprites() {
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


void game_start() {
    ESP_LOGI(TAG, "game started!");

    game_load();

    while (true) {
        time_t start_render_time = tsgl_time();

        loadSprites();
        tsgl_framebuffer_clear(&framebuffer, black);
        gfx_drawCenteredScreenImageSprite(room_sprite);
        tsgl_display_send(&display, &framebuffer);

        ESP_LOGI(TAG, "frame time %lld", tsgl_time() - start_render_time);
        tsgl_delay(10);
    }
}
