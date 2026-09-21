#include "racing.h"
#include "../gfx.h"

#define GAME_ZONE (WIDTH - (WIDTH / 4))
#define COLOR_GRASS tsgl_color_raw(tsgl_color_fromHex(0x166633), framebuffer.colormode)

#define START_CAR_POS_X (GAME_ZONE / 2)

typedef struct {
    tsgl_sprite* car_sprite;
    tsgl_pos car_x;
    tsgl_pos car_y;
    tsgl_pos size_x;
    tsgl_pos size_y;
} Subgame_state;

Subgame_state* subgame_state = NULL;

void subgame_racing_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->car_sprite = gfx_loadSprite("/firmware/images/gamecar.bmp");

    subgame_state->size_x = subgame_state->car_sprite->sprite->width;
    subgame_state->size_y = subgame_state->car_sprite->sprite->height;

    subgame_state->car_x = GAME_ZONE / 2;
    subgame_state->car_y = HEIGHT - (subgame_state->size_y / 2) - 10;
}

static void game_exit() {
    tsgl_bmp_free(subgame_state->car_sprite);
    subgame_state->car_sprite = NULL;

    free(subgame_state);
    subgame_state = NULL;

    game_alt_handle = NULL;
}

void subgame_racing_handle() {
    tsgl_framebuffer_fill(&framebuffer, 0, 0, GAME_ZONE, HEIGHT, COLOR_GRASS);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, WIDTH - GAME_ZONE, HEIGHT, black);

    tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
    tsgl_pos car_y = subgame_state->car_y - (subgame_state->size_y / 2);
    if (car_x >= 0 && car_y >= 0
        && car_x <= (WIDTH - subgame_state->size_x)
        && car_y <= (HEIGHT - subgame_state->size_y))
        PUSH_FUNC_TRANS(&framebuffer, car_x, car_y, subgame_state->car_sprite);
}
