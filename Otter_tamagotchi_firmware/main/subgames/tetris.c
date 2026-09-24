#include "tetris.h"
#include "../gfx.h"
#include "../pushsound.h"
#include "../game/game_modal.h"
#include "../game/game_printsets.h"
#include "../game/game_states.h"

// ----------------------------------------------------------

#define BURNLINE_ADD_SCORE 10
#define BURNLINE_ADD_MONEY 10

#define WIREFRAME_STOKE_SIZE 1
#define BLOCKSIZE 8
#define GAMEARRAY_X 10
#define GAMEARRAY_Y (HEIGHT / BLOCKSIZE) //20

#define STATUS_ZONE (WIDTH - (GAMEARRAY_X * BLOCKSIZE))
#define GAME_ZONE (WIDTH - STATUS_ZONE)
#define SEPARATOR_LINE_SIZE 2

#define BG_COLOR tsgl_color_raw(tsgl_color_fromHex(0x333333), framebuffer.colormode)

static const char* music_path = "/firmware/music/tetris.nbs";
#define MUSIC_VOLUME 1

static const char* sound_gameover_path = "/firmware/sounds/gameover.pcm";
#define SOUND_GAMEOVER_SAMPLERATE 16000
#define SOUND_GAMEOVER_VOLUME 1

#define DEFAULT_SCORE_DELTA 1
#define DEFAULT_STEPS_PER_SECOND 1

#define SADNESS_DELTA -0.05 //при полностью заполненой шкале печали она уйдет за 2000 секунд. то есть за 33 минуты игры

#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 25

#define OBJECT_X 4
#define OBJECT_Y 4

#define WIREFRAME_SIZE_X (BLOCKSIZE * OBJECT_X)
#define WIREFRAME_SIZE_Y (BLOCKSIZE * OBJECT_Y)

#define WITHOUT_MUSIC true

#define rgb tsgl_rgb
const tsgl_color blockcolors[] = {
    rgb(253, 62, 62),
    rgb(174, 0, 0),

    rgb(255, 133, 58),
    rgb(181, 99, 0),

    rgb(251, 232, 58),
    rgb(186, 167, 0),

    rgb(93, 255, 64),
    rgb(14, 170, 0),

    rgb(64, 220, 255),
    rgb(0, 168, 176),

    rgb(64, 83, 255),
    rgb(0, 0, 162),

    rgb(255, 66, 239),
    rgb(167, 0, 142),
};
#define COLOR_COUNT TSGL_CALC_ARRSIZE(blockcolors)

// ----------------------------------------------------------

typedef struct {
    uint8_t object_index;
    uint8_t local_index;
    uint8_t array[OBJECT_Y][OBJECT_X];
    bool reverse_color_palette;
} Tetris_object;

#include "cparts/tetris_objects.h"
#define BASE_OBJECTS_COUNT TSGL_CALC_ARRSIZE(base_objects)

// ----------------------------------------------------------

typedef struct {
    bool gameover;
    time_t oldTimerTickTime;
    time_t oldTimerStepTime;
    int steps_per_second;
    
    int score;
    int score_delta;

    tsgl_nbs* music;
    tsgl_sprite* person_sprite;

    uint8_t gamearray[GAMEARRAY_X][GAMEARRAY_Y];
    tsgl_rawcolor blockcolors[COLOR_COUNT];

    tsgl_pos current_object_x;
    tsgl_pos current_object_y;

    Tetris_object current_object;
    Tetris_object next_object;
} Subgame_state;

static Subgame_state* subgame_state = NULL;

static Tetris_object get_random_object() {
    const Tetris_object base_object = base_objects[tsgl_random(0, BASE_OBJECTS_COUNT - 1)];
    uint8_t random_color = tsgl_random(0, (COLOR_COUNT / 2) - 1) * 2;

    Tetris_object tetris_object;
    memcpy(&tetris_object, &base_object, sizeof(Tetris_object));

    uint8_t chess_offset = tetris_object.reverse_color_palette;
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            if (tetris_object.array[iy][ix] > 0) {
                uint8_t color_offset = (ix + iy + chess_offset) % 2;
                tetris_object.array[iy][ix] = (random_color + color_offset) + 1;
            }
        }
    }

    return tetris_object;
}

static Tetris_object paint_object(Tetris_object object, uint8_t color) {
    color = (color / 2) * 2;

    uint8_t chess_offset = object.reverse_color_palette;
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            if (object.array[iy][ix] > 0) {
                uint8_t color_offset = (ix + iy + chess_offset) % 2;
                object.array[iy][ix] = (color + color_offset) + 1;
            }
        }
    }

    return object;
}

static uint8_t get_color(Tetris_object object) {
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            if (object.array[iy][ix] > 0) {
                return object.array[iy][ix] - 1;
            }
        }
    }

    return 0;
}

static Tetris_object rotate_object(Tetris_object object) {
    uint8_t variants_count = 0;
    for (size_t i = 0; i < BASE_OBJECTS_COUNT; i++) {
        Tetris_object obj = base_objects[i];
        if (obj.object_index == object.object_index) {
            if (obj.local_index > variants_count) variants_count = obj.local_index;
        }
    }
    variants_count++;

    uint8_t next_local_index = (object.local_index + 1) % variants_count;
    for (size_t i = 0; i < BASE_OBJECTS_COUNT; i++) {
        Tetris_object obj = base_objects[i];
        if (obj.local_index == next_local_index && obj.object_index == object.object_index) {
            return paint_object(obj, get_color(object));
        }
    }

    ESP_LOGE(TAG, "Object not found");
    return object;
}

static void print_tetris_object(tsgl_pos x, tsgl_pos y, Tetris_object tetris_object) {
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            uint8_t type = tetris_object.array[iy][ix];
            if (type > 0) {
                size_t nx = x + ix;
                size_t ny = y + iy;
                if (nx < GAMEARRAY_X && ny < GAMEARRAY_Y)
                    subgame_state->gamearray[nx][ny] = type;
            }
        }
    }
}

static bool check_collision(tsgl_pos x, tsgl_pos y, Tetris_object tetris_object) {
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            uint8_t type = tetris_object.array[iy][ix];
            if (type > 0) {
                size_t nx = x + ix;
                size_t ny = y + iy;
                if (nx < GAMEARRAY_X && ny < GAMEARRAY_Y && subgame_state->gamearray[nx][ny] > 0)
                    return true;
            }
        }
    }

    return false;
}

static void draw_tetris_object(tsgl_pos x, tsgl_pos y, Tetris_object tetris_object) {
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            uint8_t type = tetris_object.array[iy][ix];
            if (type > 0) {
                tsgl_framebuffer_fill(&framebuffer, x + (ix * BLOCKSIZE), y + (iy * BLOCKSIZE), BLOCKSIZE, BLOCKSIZE, subgame_state->blockcolors[type - 1]);
            }
        }
    }
}

static void get_tetris_object_size(Tetris_object tetris_object, tsgl_pos* sizeX, tsgl_pos* sizeY) {
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            uint8_t type = tetris_object.array[iy][ix];
            if (type > 0) {
                tsgl_pos sx = ix + 1;
                tsgl_pos sy = iy + 1;
                if (sx > *sizeX) *sizeX = sx;
                if (sy > *sizeY) *sizeY = sy;
            }
        }
    }
}

static void draw_wireframe(tsgl_pos x, tsgl_pos y, Tetris_object tetris_object) {
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            tsgl_framebuffer_rect(&framebuffer, x + (ix * BLOCKSIZE), y + (iy * BLOCKSIZE), BLOCKSIZE, BLOCKSIZE, white, WIREFRAME_STOKE_SIZE);
        }
    }

    tsgl_pos sizeX = 0;
    tsgl_pos sizeY = 0;
    get_tetris_object_size(tetris_object, &sizeX, &sizeY);

    tsgl_pos blockOffsetX = (OBJECT_X / 2) - (sizeX / 2);
    tsgl_pos blockOffsetY = (OBJECT_Y / 2) - (sizeY / 2);
    draw_tetris_object(x + (blockOffsetX * BLOCKSIZE), y + (blockOffsetY * BLOCKSIZE), tetris_object);
}

static void next_object() {
    subgame_state->current_object = subgame_state->next_object;

    tsgl_pos sizeX = 0;
    tsgl_pos sizeY = 0;
    get_tetris_object_size(subgame_state->current_object, &sizeX, &sizeY);
    
    subgame_state->current_object_x = tsgl_random(0, GAMEARRAY_X - sizeX);
    subgame_state->current_object_y = -sizeY;

    subgame_state->next_object = get_random_object();
}

void subgame_tetris_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    if (!WITHOUT_MUSIC) {
        subgame_state->music = pushsound_nbs_loop(music_path, MUSIC_VOLUME);
    }
    subgame_state->person_sprite = game_getPersonSprite();
    subgame_state->oldTimerTickTime = tsgl_time();
    subgame_state->oldTimerStepTime = subgame_state->oldTimerTickTime;
    subgame_state->score_delta = DEFAULT_SCORE_DELTA;
    subgame_state->steps_per_second = DEFAULT_STEPS_PER_SECOND;

    subgame_state->current_object = get_random_object();
    subgame_state->next_object = get_random_object();

    for (size_t i = 0; i < COLOR_COUNT; i++) {
        subgame_state->blockcolors[i] = tsgl_color_raw(blockcolors[i], framebuffer.colormode);
    }
}

static void stop_music() {
    if (subgame_state->music != NULL) {
        tsgl_nbs_free(subgame_state->music);
        subgame_state->music = NULL;
    }
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

static void draw_array() {
    for (size_t ix = 0; ix < GAMEARRAY_X; ix++) {
        for (size_t iy = 0; iy < GAMEARRAY_Y; iy++) {
            uint8_t type = subgame_state->gamearray[ix][iy];
            if (type > 0) {
                tsgl_framebuffer_fill(&framebuffer, ix * BLOCKSIZE, iy * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, subgame_state->blockcolors[type - 1]);
            }
        }
    }
}

static void draw_current_object() {
    tsgl_pos target_x = subgame_state->current_object_x * BLOCKSIZE;
    tsgl_pos target_y = subgame_state->current_object_y * BLOCKSIZE;
    draw_tetris_object(target_x, target_y, subgame_state->current_object);
}

static void after_burn_line() {
    subgame_state->score += BURNLINE_ADD_SCORE;
    current_state.states_money += BURNLINE_ADD_MONEY;
    pushsound_play("/firmware/sounds/money.pcm", 16000, MONEY_SOUND_VOLUME);
}

static void burn_line(tsgl_pos posY) {
    for (int ix = 0; ix < GAMEARRAY_X; ix++) {
        for (int iy = posY - 1; iy >= 0; iy--) {
            subgame_state->gamearray[ix][iy + 1] = subgame_state->gamearray[ix][iy];
        }
        subgame_state->gamearray[ix][0] = 0;
    }
    after_burn_line();
}

static void burn_line_check() {
    for (int iy = GAMEARRAY_Y - 1; iy >= 0;) {
        bool found_line = true;
        for (int ix = 0; ix < GAMEARRAY_X; ix++) {
            uint8_t type = subgame_state->gamearray[ix][iy];
            if (type == 0) {
                found_line = false;
                break;
            }
        }

        if (found_line) {
            burn_line(iy);
            break; //одну линию за раз
        } else {
            iy--;
        }
    }
}

static void weld_object() {
    print_tetris_object(subgame_state->current_object_x, subgame_state->current_object_y, subgame_state->current_object);
    next_object();
    pushsound_play("/firmware/sounds/weld.pcm", 16000, WELD_SOUND_VOLUME);
}

static void fall_object() {
    tsgl_pos sizeX = 0;
    tsgl_pos sizeY = 0;
    get_tetris_object_size(subgame_state->current_object, &sizeX, &sizeY);

    if (subgame_state->current_object_y + sizeY >= GAMEARRAY_Y || check_collision(subgame_state->current_object_x, subgame_state->current_object_y + 1, subgame_state->current_object)) {
        weld_object();
    } else {
        subgame_state->current_object_y++;
    }
}

static void border_check() {
    tsgl_pos sizeX = 0;
    tsgl_pos sizeY = 0;
    get_tetris_object_size(subgame_state->current_object, &sizeX, &sizeY);

    tsgl_pos max_x = GAMEARRAY_X - sizeX;
    if (subgame_state->current_object_x > max_x) subgame_state->current_object_x = max_x;
}

static void process() {
    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_LEFT)) {
        if (!check_collision(subgame_state->current_object_x - 1, subgame_state->current_object_y, subgame_state->current_object)) {
            subgame_state->current_object_x--;
            if (subgame_state->current_object_x < 0) subgame_state->current_object_x = 0;
        }
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_OKAY)) {
        Tetris_object rotated = rotate_object(subgame_state->current_object);
        if (!check_collision(subgame_state->current_object_x, subgame_state->current_object_y, rotated)) {
            subgame_state->current_object = rotated;
            border_check();
        }
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_RIGHT)) {
        if (!check_collision(subgame_state->current_object_x + 1, subgame_state->current_object_y, subgame_state->current_object)) {
            subgame_state->current_object_x++;
            border_check();
        }
    }
}

void subgame_tetris_handle() {
    if (subgame_state->gameover) {
        if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
            game_exit();
            return;
        }

        game_modal_draw_gameover(subgame_state->score, current_state.subgame_tetris_max_score);
        return;
    }

    time_t currentTime = tsgl_time();
    if (currentTime - subgame_state->oldTimerTickTime > 1000) {
        subgame_state->oldTimerTickTime = currentTime;

        game_states_change(&current_state.states_sadness, SADNESS_DELTA);

        subgame_state->score += subgame_state->score_delta;
    }

    if (currentTime - subgame_state->oldTimerStepTime > (1000 / subgame_state->steps_per_second)) {
        subgame_state->oldTimerStepTime = currentTime;
        fall_object();
        burn_line_check();
    } else if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
        fall_object();
    }

    process();

    tsgl_framebuffer_fill(&framebuffer, 0, 0, GAME_ZONE, HEIGHT, BG_COLOR);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, STATUS_ZONE, HEIGHT, black);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, SEPARATOR_LINE_SIZE, HEIGHT, white);

    draw_array();
    draw_current_object();

    printsettings_subgames.fg = white;
    printsettings_subgames.width = STATUS_ZONE;
    printsettings_subgames.height = PRINT_GAP_Y;

    tsgl_pos draw_y = PRINT_START_POS_Y;
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "SCORE\n%i", subgame_state->score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    if (subgame_state->score > current_state.subgame_tetris_max_score)
        current_state.subgame_tetris_max_score = subgame_state->score;

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "HIGH\n%i", current_state.subgame_tetris_max_score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    PUSH_FUNC_TRANS(&framebuffer,
        (GAME_ZONE + (STATUS_ZONE / 2)) - (subgame_state->person_sprite->sprite->width / 2),
        HEIGHT - subgame_state->person_sprite->sprite->height - 2,
        subgame_state->person_sprite
    );

    tsgl_pos margin = (STATUS_ZONE - WIREFRAME_SIZE_X) / 2;
    draw_wireframe((GAME_ZONE + (STATUS_ZONE / 2)) - (WIREFRAME_SIZE_X / 2), HEIGHT - WIREFRAME_SIZE_Y - margin, subgame_state->next_object);
}
