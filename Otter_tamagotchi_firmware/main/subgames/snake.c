#include "snake.h"
#include "../gfx.h"
#include "../pushsound.h"
#include "../game/game_modal.h"
#include "../game/game_printsets.h"
#include "../game/game_states.h"

// ----------------------------------------------------------

#define BLOCKSIZE 8
#define GAMEARRAY_X 10
#define GAMEARRAY_Y (HEIGHT / BLOCKSIZE) //20

#define BLOCK_MARGIN 1

#define STATUS_ZONE (WIDTH - (GAMEARRAY_X * BLOCKSIZE))
#define GAME_ZONE (WIDTH - STATUS_ZONE)
#define SEPARATOR_LINE_SIZE 2

#define BG_COLOR tsgl_color_raw(tsgl_color_fromHex(0x1a4e01), framebuffer.colormode)
#define SNAKE_COLOR tsgl_color_raw(tsgl_color_fromHex(0x43be09), framebuffer.colormode)
#define SNAKE_HEAD_COLOR yellow
#define EAT_COLOR red

static const char* music_path = "/firmware/music/edmvselo.dpw";
#define MUSIC_SAMPLERATE 16000
#define MUSIC_VOLUME 0.6

static const char* sound_gameover_path = "/firmware/sounds/gameover.pcm";
#define SOUND_GAMEOVER_SAMPLERATE 16000
#define SOUND_GAMEOVER_VOLUME 1

static const char* sound_win_path = "/firmware/sounds/gameover.pcm";
#define SOUND_WIN_SAMPLERATE 16000
#define SOUND_WIN_VOLUME 1

#define DEFAULT_SCORE_DELTA 1

#define SADNESS_DELTA -0.05

#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 25

#define GAMEARRAY_EAT_ID 254
#define GAMEARRAY_HEAD_ID 255

#define EAT_COUNT 2

#define DEFAULT_SNAKE_DIRECTION 1

// ----------------------------------------------------------

typedef struct {
    bool gameover;
    bool win;
    time_t oldTimerTickTime;
    time_t oldTimerMove;
    
    int score;
    int score_delta;

    tsgl_sound* music;
    tsgl_sprite* person_sprite;

    uint8_t snake_direction;
    uint8_t snake_len;

    uint8_t gamearray[GAMEARRAY_X][GAMEARRAY_Y];
} Subgame_state;

static Subgame_state* subgame_state = NULL;

static void stop_music() {
    if (subgame_state->music != NULL) {
        tsgl_sound_free(subgame_state->music);
        subgame_state->music = NULL;
    }
}

static void win() {
    stop_music();
    pushsound_play(sound_win_path, SOUND_WIN_SAMPLERATE, SOUND_WIN_VOLUME);
    subgame_state->win = true;
}

static void spawn_eat() {
    uint8_t eatId = GAMEARRAY_EAT_ID;

    size_t maxIters = GAMEARRAY_X * GAMEARRAY_Y * 10;
    for (size_t i = 0; i < maxIters; i++) {
        tsgl_pos px = tsgl_random(0, GAMEARRAY_X);
        tsgl_pos py = tsgl_random(0, GAMEARRAY_Y);
    
        if (subgame_state->gamearray[px][py] == 0) {
            subgame_state->gamearray[px][py] = eatId;
            return;
        }
    }

    for (size_t px = 0; px < GAMEARRAY_X; px++) {
        for (size_t py = 0; py < GAMEARRAY_Y; py++) {
            if (subgame_state->gamearray[px][py] == 0) {
                subgame_state->gamearray[px][py] = eatId;
                return;
            }
        }
    }

    // выигрываем если заспавнить еду тупо некуда
    win();
}

static void fill_default_gamearray() {
    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            subgame_state->gamearray[ix][iy] = 0;
        }
    }

    tsgl_pos px = GAMEARRAY_X / 2;
    tsgl_pos py = GAMEARRAY_Y / 2;
    subgame_state->gamearray[px][py] = GAMEARRAY_HEAD_ID;
    subgame_state->gamearray[px][py + 1] = 1;

    for (size_t i = 0; i < EAT_COUNT; i++) {
        spawn_eat();
    }
}

void subgame_snake_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    time_t currentTime = tsgl_time();

    subgame_state->music = pushsound_loop(music_path, MUSIC_SAMPLERATE, MUSIC_VOLUME);
    subgame_state->person_sprite = game_getPersonSprite();
    subgame_state->oldTimerTickTime = currentTime;
    subgame_state->oldTimerMove = currentTime;
    subgame_state->score_delta = DEFAULT_SCORE_DELTA;
    subgame_state->snake_direction = DEFAULT_SNAKE_DIRECTION;

    fill_default_gamearray();
}

static void game_exit() {
    stop_music();

    free(subgame_state);
    subgame_state = NULL;

    game_alt_handle = NULL;
}

static void gameover() {
    stop_music();
    pushsound_play(sound_gameover_path, SOUND_GAMEOVER_SAMPLERATE, SOUND_GAMEOVER_VOLUME);
    subgame_state->gameover = true;
}

static void snakeCollision(uint8_t collisionWith) {
    if (collisionWith == GAMEARRAY_EAT_ID) {
        subgame_state->snake_len++;
    } else if (collisionWith > 0) {
        gameover();
    }
}

static void moveSnakeSpawnHead(tsgl_pos x, tsgl_pos y) {
    tsgl_pos nx = x;
    tsgl_pos ny = y;

    switch (subgame_state->snake_direction) {
        case 0:
            nx--;
            break;

        case 1:
            ny--;
            break;

        case 2:
            ny++;
            break;

        case 3:
            nx++;
            break;
    }

    if (nx < 0) nx = GAMEARRAY_X - 1;
    else if (nx >= GAMEARRAY_X) nx = 0;

    if (ny < 0) nx = GAMEARRAY_Y - 1;
    else if (ny >= GAMEARRAY_Y) ny = 0;

    subgame_state->gamearray[x][y] = subgame_state->snake_len;
    snakeCollision(subgame_state->gamearray[nx][ny]);
    subgame_state->gamearray[nx][ny] = GAMEARRAY_HEAD_ID;
}

static void processSnake() {
    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            uint8_t snake = subgame_state->gamearray[ix][iy];

            if (snake == GAMEARRAY_HEAD_ID) {
                moveSnakeSpawnHead(ix, iy);
            } else if (snake != GAMEARRAY_EAT_ID && snake > 0) {
                subgame_state->gamearray[ix][iy] -= 1;
            }
        }
    }
}

static void snakeMoveDirect() {
    subgame_state->oldTimerMove = tsgl_time();
    processSnake();
}

static void snakeMoveFromKeyboard() {
    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_LEFT)) {
        subgame_state->snake_direction = 0;
        snakeMoveDirect();
    }

    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_OKAY)) {
        subgame_state->snake_direction = 1;
        snakeMoveDirect();
    }

    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_CANCEL)) {
        subgame_state->snake_direction = 2;
        snakeMoveDirect();
    }

    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_RIGHT)) {
        subgame_state->snake_direction = 3;
        snakeMoveDirect();
    }
}

static void drawSnakeBlock(tsgl_pos x, tsgl_pos y, tsgl_rawcolor color) {
    tsgl_pos posX = x * BLOCKSIZE;
    tsgl_pos posY = y * BLOCKSIZE;

    posX += BLOCK_MARGIN;
    posY += BLOCK_MARGIN;

    tsgl_framebuffer_fill(&framebuffer, posX, posY, BLOCKSIZE - (BLOCK_MARGIN * 2), BLOCKSIZE - (BLOCK_MARGIN * 2), color);
}

static void drawSnake() {
    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            uint8_t snake = subgame_state->gamearray[ix][iy];

            switch (snake) {
                case GAMEARRAY_HEAD_ID:
                    drawSnakeBlock(ix, iy, SNAKE_HEAD_COLOR);
                    break;

                case GAMEARRAY_EAT_ID:
                    drawSnakeBlock(ix, iy, EAT_COLOR);
                    break;
                
                default:
                    drawSnakeBlock(ix, iy, SNAKE_COLOR);
                    break;
            }
        }
    }
}

void subgame_snake_handle() {
    if (tsgl_keyboard_whenHold(&keyboard, KEY_INDEX_LEFT) && tsgl_keyboard_whenHold(&keyboard, KEY_INDEX_RIGHT)) {
        game_exit();
        return;
    }

    if (subgame_state->win) {
        if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
            game_exit();
            return;
        }
        
        game_modal_draw_win(subgame_state->score, current_state.subgame_snake_max_score);
        return;
    }

    if (subgame_state->gameover) {
        if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
            game_exit();
            return;
        }
        
        game_modal_draw_gameover(subgame_state->score, current_state.subgame_snake_max_score);
        return;
    }

    snakeMoveFromKeyboard();

    time_t currentTime = tsgl_time();
    if (currentTime - subgame_state->oldTimerMove > 1000) {
        snakeMoveDirect();
    }

    if (currentTime - subgame_state->oldTimerTickTime > 1000) {
        subgame_state->oldTimerTickTime = currentTime;

        game_states_change(&current_state.states_sadness, SADNESS_DELTA);

        subgame_state->score += subgame_state->score_delta;
    }
    
    if (subgame_state->score > current_state.subgame_snake_max_score)
        current_state.subgame_snake_max_score = subgame_state->score;

    tsgl_framebuffer_fill(&framebuffer, 0, 0, GAME_ZONE, HEIGHT, BG_COLOR);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, STATUS_ZONE, HEIGHT, black);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, SEPARATOR_LINE_SIZE, HEIGHT, white);

    drawSnake();

    printsettings_subgames.fg = white;
    printsettings_subgames.width = STATUS_ZONE;
    printsettings_subgames.height = PRINT_GAP_Y;

    tsgl_pos draw_y = PRINT_START_POS_Y;
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "SCORE\n%i", subgame_state->score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "HIGH\n%i", current_state.subgame_snake_max_score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    PUSH_FUNC_TRANS(&framebuffer,
        (GAME_ZONE + (STATUS_ZONE / 2)) - (subgame_state->person_sprite->fb->width / 2),
        HEIGHT - subgame_state->person_sprite->fb->height - 2,
        subgame_state->person_sprite
    );
}

void subgame_snake_exit() {
    game_exit();
}
