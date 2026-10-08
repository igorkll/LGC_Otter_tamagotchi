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
#define EMPTY_COLOR tsgl_color_raw(tsgl_color_fromHex(0x267102), framebuffer.colormode)
#define SNAKE_COLOR green
#define SNAKE_HEAD_COLOR yellow
#define EAT_COLOR red
#define BESTEAT_0_COLOR blue
#define BESTEAT_1_COLOR magenta

static const char* music_path = "/firmware/music/edmvselo.dpw";
#define MUSIC_SAMPLERATE SOUND_DFPWM_SAMPLERATE
#define MUSIC_VOLUME 0.6

static const char* sound_gameover_path = "/firmware/sounds/gameover.pcm";
#define SOUND_GAMEOVER_SAMPLERATE SOUND_EFFECTS_SAMPLERATE
#define SOUND_GAMEOVER_VOLUME 1

static const char* sound_win_path = "/firmware/sounds/win.pcm";
#define SOUND_WIN_SAMPLERATE SOUND_EFFECTS_SAMPLERATE
#define SOUND_WIN_VOLUME 1

static const char* sound_eat_path = "/firmware/sounds/pickup.pcm";
#define SOUND_EAT_SAMPLERATE SOUND_EFFECTS_SAMPLERATE
#define SOUND_EAT_VOLUME PICKUP_SOUND_VOLUME

static const char* sound_money_path = "/firmware/sounds/money.pcm";

#define DEFAULT_SCORE_DELTA 1

#define SADNESS_DELTA -0.05

#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 25

#define EAT_COUNT 2

#define DEFAULT_SNAKE_DIRECTION 1

#define DEFAULT_SNAKE_LEN 2

#define EAT_SCORE_ADD 10
#define WIN_MONEY_ADD 1000

// этот типо должен быть больше чем GAMEARRAY_X*GAMEARRAY_Y на количество специальных ID обьявленых ниже
typedef uint8_t snake_t;
#define GAMEARRAY_BESTEAT_ID 253
#define GAMEARRAY_EAT_ID 254
#define GAMEARRAY_HEAD_ID 255

#define GAMEARRAY_MIN_ID GAMEARRAY_BESTEAT_ID

#define BEATEAT_MUSIC_SPEED 1.2
#define BEST_SPAWN_RND 60
#define MAX_BEATEAT_FRAMES 100

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

    bool best_eat;
    uint16_t beat_eat_frames;

    uint8_t snake_direction;

    snake_t snake_len;
    snake_t gamearray[GAMEARRAY_X][GAMEARRAY_Y];

    uint64_t frame;
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

    pushsound_play(sound_money_path, SOUND_EFFECTS_SAMPLERATE, MONEY_SOUND_VOLUME);
    current_state.states_money += WIN_MONEY_ADD;
}

static void spawn_eat(bool best) {
    snake_t eatId = 0;
    if (best) {
        subgame_state->beat_eat_frames = 0;
        subgame_state->best_eat = true;
        eatId = GAMEARRAY_BESTEAT_ID;
    } else {
        eatId = GAMEARRAY_EAT_ID;
    }

    size_t maxIters = GAMEARRAY_X * GAMEARRAY_Y * 10;
    for (size_t i = 0; i < maxIters; i++) {
        tsgl_pos px = tsgl_random(0, GAMEARRAY_X - 1);
        tsgl_pos py = tsgl_random(0, GAMEARRAY_Y - 1);
    
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

    for (size_t i = 0; i < subgame_state->snake_len; i++) {
        subgame_state->gamearray[px][py + 1 + i] = subgame_state->snake_len - i;
    }

    for (size_t i = 0; i < EAT_COUNT; i++) {
        spawn_eat(false);
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
    subgame_state->snake_len = DEFAULT_SNAKE_LEN;

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

static void snake_addLen(int add) {
    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            snake_t snake = subgame_state->gamearray[ix][iy];
            
            if (snake > 0 && snake < GAMEARRAY_MIN_ID) {
                subgame_state->gamearray[ix][iy] += add;
            }
        }
    }

    subgame_state->snake_len += add;
    subgame_state->score += EAT_SCORE_ADD;
}

static bool snakeCollision(snake_t collisionWith) {
    if (collisionWith == GAMEARRAY_EAT_ID) {
        pushsound_play(sound_eat_path, SOUND_EAT_SAMPLERATE, SOUND_EAT_VOLUME);
        snake_addLen(1);
        return true;
    } else if (collisionWith == GAMEARRAY_BESTEAT_ID) {
        subgame_state->best_eat = false;
        pushsound_play(sound_money_path, SOUND_EFFECTS_SAMPLERATE, MONEY_SOUND_VOLUME);
        snake_addLen(5);
    } else if (collisionWith > 0) {
        gameover();
    }

    return false;
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

    if (ny < 0) ny = GAMEARRAY_Y - 1;
    else if (ny >= GAMEARRAY_Y) ny = 0;

    bool spawnEat = snakeCollision(subgame_state->gamearray[nx][ny]);
    subgame_state->gamearray[x][y] = subgame_state->snake_len + 1;
    subgame_state->gamearray[nx][ny] = GAMEARRAY_HEAD_ID;
    if (spawnEat) spawn_eat(false);
}

static void processSnake() {
    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        bool doubleBreak = false;
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            snake_t snake = subgame_state->gamearray[ix][iy];

            if (snake == GAMEARRAY_HEAD_ID) {
                moveSnakeSpawnHead(ix, iy);
                doubleBreak = true;
                break;
            }
        }
        if (doubleBreak) break;
    }

    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            snake_t snake = subgame_state->gamearray[ix][iy];

            if (snake > 0 && snake < GAMEARRAY_MIN_ID) {
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
    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_LEFT) && subgame_state->snake_direction != 3) {
        subgame_state->snake_direction = 0;
        snakeMoveDirect();
    }

    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_OKAY) && subgame_state->snake_direction != 2) {
        subgame_state->snake_direction = 1;
        snakeMoveDirect();
    }

    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_CANCEL) && subgame_state->snake_direction != 1) {
        subgame_state->snake_direction = 2;
        snakeMoveDirect();
    }

    if (tsgl_keyboard_whenPressedOrHold(&keyboard, KEY_INDEX_RIGHT) && subgame_state->snake_direction != 0) {
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
            snake_t snake = subgame_state->gamearray[ix][iy];

            if (snake == GAMEARRAY_HEAD_ID) {
                drawSnakeBlock(ix, iy, SNAKE_HEAD_COLOR);
            } else if (snake == GAMEARRAY_EAT_ID) {
                drawSnakeBlock(ix, iy, EAT_COLOR);
            } else if (snake == GAMEARRAY_BESTEAT_ID) {
                drawSnakeBlock(ix, iy, subgame_state->frame % 10 >= 5 ? BESTEAT_0_COLOR : BESTEAT_1_COLOR);
            } else if (snake > 0) {
                drawSnakeBlock(ix, iy, SNAKE_COLOR);
            } else {
                drawSnakeBlock(ix, iy, EMPTY_COLOR);
            }
        }
    }
}

static void deleteBeateat() {
    subgame_state->beat_eat_frames = 0;
    subgame_state->best_eat = false;

    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            snake_t snake = subgame_state->gamearray[ix][iy];

            if (snake == GAMEARRAY_BESTEAT_ID) {
                subgame_state->gamearray[ix][iy] = -1;
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
        if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_CANCEL)) {
            game_exit();
            return;
        }
        
        game_modal_draw_win(subgame_state->score, current_state.subgame_snake_max_score);
        return;
    }

    if (subgame_state->gameover) {
        if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_CANCEL)) {
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

    if (subgame_state->music != NULL) {
        tsgl_sound_setSpeed(subgame_state->music, subgame_state->best_eat ? BEATEAT_MUSIC_SPEED : 1);
    }

    if (!subgame_state->best_eat) {
        if (tsgl_random(0, BEST_SPAWN_RND) == 0) spawn_eat(true);
    } else {
        subgame_state->beat_eat_frames++;
        if (subgame_state->beat_eat_frames > MAX_BEATEAT_FRAMES) {
            deleteBeateat();
        }
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

    subgame_state->frame++;
}

void subgame_snake_exit() {
    game_exit();
}
