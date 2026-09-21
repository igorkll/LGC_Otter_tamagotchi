#include "racing.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../game/game_printsets.h"

#define STATUS_ZONE 50
#define GAME_ZONE (WIDTH - STATUS_ZONE)
#define SEPARATOR_LINE_SIZE 2

#define COLOR_GRASS tsgl_color_raw(tsgl_color_fromHex(0x166633), framebuffer.colormode)

#define START_CAR_POS_X (GAME_ZONE / 2)

#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 25

#define TAXIING_SPEED 2

typedef struct {
    tsgl_sprite* car_sprite;
    tsgl_pos car_x;
    tsgl_pos car_y;
    tsgl_pos size_x;
    tsgl_pos size_y;
    
    int score;
    tsgl_pos speed;
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

static time_t oldTimerTickTime = -9999;
void subgame_racing_handle() {
    // ------------------------ process

    time_t currentTime = tsgl_time();
    if (currentTime - oldTimerTickTime > 1000) {
        oldTimerTickTime = currentTime;
        subgame_state->score++;
    }
    
    if (subgame_state->score > current_state.subgame_recing_max_score)
        current_state.subgame_recing_max_score = subgame_state->score;

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_OKAY)) {
        subgame_state->car_x -= TAXIING_SPEED;

        tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
        if (car_x < 0) {
            subgame_state->car_x = subgame_state->size_x / 2;
        }
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
        game_exit();
        return;
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_RIGHT)) {
        subgame_state->car_x += TAXIING_SPEED;
        
        tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
        if (car_x > (GAME_ZONE - subgame_state->size_x)) {
            subgame_state->car_x = GAME_ZONE - (subgame_state->size_x / 2);
        }
    }

    // ------------------------ draw

    tsgl_framebuffer_fill(&framebuffer, 0, 0, GAME_ZONE, HEIGHT, COLOR_GRASS);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, STATUS_ZONE, HEIGHT, black);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, SEPARATOR_LINE_SIZE, HEIGHT, white);

    printsettings_subgames.fg = white;
    printsettings_subgames.width = STATUS_ZONE;
    printsettings_subgames.height = PRINT_GAP_Y;

    tsgl_pos draw_y = PRINT_START_POS_Y;
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "SCORE\n%i", subgame_state->score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "HIGH\n%i", current_state.subgame_recing_max_score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
    tsgl_pos car_y = subgame_state->car_y - (subgame_state->size_y / 2);
    printf("%i\n", car_x);
    if (car_x >= 0 && car_y >= 0
        && car_x <= (GAME_ZONE - subgame_state->size_x)
        && car_y <= (HEIGHT - subgame_state->size_y))
        PUSH_FUNC_TRANS(&framebuffer, car_x, car_y, subgame_state->car_sprite);
}
