#include "tetris.h"
#include "../gfx.h"
#include "../pushsound.h"
#include "../game/game_modal.h"
#include "../game/game_printsets.h"
#include "../game/game_states.h"

// ----------------------------------------------------------

#define STATUS_ZONE 50
#define GAME_ZONE (WIDTH - STATUS_ZONE)
#define SEPARATOR_LINE_SIZE 2

#define BG_COLOR tsgl_color_raw(tsgl_color_fromHex(0x333333), framebuffer.colormode)

static const char* music_path = "/firmware/music/edmvselo.dpw";
#define MUSIC_SAMPLERATE 16000
#define MUSIC_VOLUME 0.6

static const char* sound_gameover_path = "/firmware/sounds/gameover.pcm";
#define SOUND_GAMEOVER_SAMPLERATE 16000
#define SOUND_GAMEOVER_VOLUME 1

#define DEFAULT_SCORE_DELTA 1

#define FATIGUE_DELTA 0.05
#define SADNESS_DELTA -1.1

#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 25

#define WIREFRAME_STOKE_SIZE 1
#define BLOCKSIZE 8
#define GAMEARRAY_X (GAME_ZONE / BLOCKSIZE)
#define GAMEARRAY_Y (HEIGHT / BLOCKSIZE)

#define OBJECT_X 4
#define OBJECT_Y 4

#define WIREFRAME_SIZE_X (BLOCKSIZE * OBJECT_X)

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
} Tetris_object;

#include "cparts/tetris_objects.h"
#define BASE_OBJECTS_COUNT TSGL_CALC_ARRSIZE(base_objects)

// ----------------------------------------------------------

typedef struct {
    bool gameover;
    time_t oldTimerTickTime;
    
    int score;
    int score_delta;

    tsgl_sound* music;
    tsgl_sprite* person_sprite;

    uint8_t gamearray[GAMEARRAY_X][GAMEARRAY_Y];
    tsgl_rawcolor blockcolors[COLOR_COUNT];
    Tetris_object current_object;
    Tetris_object next_object;
} Subgame_state;

static Subgame_state* subgame_state = NULL;

static Tetris_object get_random_object() {
    const Tetris_object base_object = base_objects[tsgl_random(0, BASE_OBJECTS_COUNT - 1)];
    uint8_t random_color = tsgl_random(0, (COLOR_COUNT / 2) - 1) * 2;

    Tetris_object tetris_object;
    memcpy(&tetris_object, &base_object, sizeof(Tetris_object));

    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            if (tetris_object.array[iy][ix] > 0) {
                uint8_t color_offset = (ix + iy) % 2;
                tetris_object.array[iy][ix] = (random_color + color_offset) + 1;
            }
        }
    }

    return tetris_object;
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
            return obj;
        }
    }

    ESP_LOGE(TAG, "WTF!");
    return object;
}

static void print_tetris_object(tsgl_pos x, tsgl_pos y, Tetris_object tetris_object) {
    for (size_t ix = 0; ix < OBJECT_X; ix++) {
        for (size_t iy = 0; iy < OBJECT_Y; iy++) {
            uint8_t type = tetris_object.array[iy][ix];
            if (type > 0) {
                subgame_state->gamearray[x + ix][y + iy] = type;
            }
        }
    }
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
    subgame_state->next_object = get_random_object();
}

void subgame_tetris_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->music = pushsound_loop(music_path, MUSIC_SAMPLERATE, MUSIC_VOLUME);
    subgame_state->person_sprite = game_getPersonSprite();
    subgame_state->oldTimerTickTime = tsgl_time();
    subgame_state->score_delta = DEFAULT_SCORE_DELTA;

    subgame_state->current_object = get_random_object();
    subgame_state->next_object = get_random_object();

    for (size_t i = 0; i < COLOR_COUNT; i++) {
        subgame_state->blockcolors[i] = tsgl_color_raw(blockcolors[i], framebuffer.colormode);
    }
}

static void stop_music() {
    if (subgame_state->music != NULL) {
        tsgl_sound_free(subgame_state->music);
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

void subgame_tetris_handle() {
    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
        game_exit();
        return;
    }

    if (subgame_state->gameover) {
        game_modal_draw_gameover(subgame_state->score, current_state.subgame_tetris_max_score);
        return;
    }

    time_t currentTime = tsgl_time();
    if (currentTime - subgame_state->oldTimerTickTime > 1000) {
        subgame_state->oldTimerTickTime = currentTime;

        game_states_change(&current_state.states_fatigue, FATIGUE_DELTA);
        game_states_change(&current_state.states_sadness, SADNESS_DELTA);

        subgame_state->score += subgame_state->score_delta;
    }
    
    if (subgame_state->score > current_state.subgame_tetris_max_score)
        current_state.subgame_tetris_max_score = subgame_state->score;

    tsgl_framebuffer_fill(&framebuffer, 0, 0, GAME_ZONE, HEIGHT, BG_COLOR);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, STATUS_ZONE, HEIGHT, black);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, SEPARATOR_LINE_SIZE, HEIGHT, white);

    draw_array();

    printsettings_subgames.fg = white;
    printsettings_subgames.width = STATUS_ZONE;
    printsettings_subgames.height = PRINT_GAP_Y;

    tsgl_pos draw_y = PRINT_START_POS_Y;
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "SCORE\n%i", subgame_state->score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "HIGH\n%i", current_state.subgame_tetris_max_score);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    PUSH_FUNC_TRANS(&framebuffer,
        (GAME_ZONE + (STATUS_ZONE / 2)) - (subgame_state->person_sprite->sprite->width / 2),
        HEIGHT - subgame_state->person_sprite->sprite->height - 2,
        subgame_state->person_sprite
    );

    draw_wireframe((GAME_ZONE + (STATUS_ZONE / 2)) - (WIREFRAME_SIZE_X / 2), draw_y, get_random_object());

    
    draw_tetris_object(20, 50, subgame_state->current_object);
    draw_tetris_object(50, 50, subgame_state->next_object);
    draw_tetris_object(50, 20, get_random_object());
    
    tsgl_delay(1000);
}
