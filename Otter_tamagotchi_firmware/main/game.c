#include "game.h"

void game_start() {
    ESP_LOGI(TAG, "game started!");

    while (true) {
        tsgl_framebuffer_clear(&framebuffer, white);
        tsgl_display_send(&display, &framebuffer);
        tsgl_delay(10);
    }
}