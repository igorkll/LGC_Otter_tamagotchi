#include "tetris.h"
#include "../gfx.h"
#include "../pushsound.h"
#include "../game/game_modal.h"
#include "../game/game_printsets.h"
#include "../game/game_states.h"

// ----------------------------------------------------------

#define BG_COLOR tsgl_color_raw(tsgl_color_fromHex(0x2380a0), framebuffer.colormode)

static const char* music_path = "/firmware/music/edmvselo.dpw";
#define MUSIC_SAMPLERATE 16000
#define MUSIC_VOLUME 0.6

static const char* sound_gameover_path = "/firmware/sounds/gameover.pcm";
#define SOUND_GAMEOVER_SAMPLERATE 16000
#define SOUND_GAMEOVER_VOLUME 1

#define DEFAULT_SCORE_DELTA 1

#define SADNESS_DELTA -0.05

#define PRINT_START_POS_X 5
#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 10

#define MAX_OBJECTS 32

// ----------------------------------------------------------

typedef struct {
    const char* path;
} Gameobj_setup;

typedef struct {
    tsgl_pos x;
    tsgl_pos y;
    int8_t type;
    tsgl_sprite* sprite;
} Gameobj_state;

static const Gameobj_setup objects_settings[] = {
    {
        .path = "/firmware/subgames/island/cloud0.bmp",
    },
    {
        .path = "/firmware/subgames/island/cloud1.bmp",
    },
    {
        .path = "/firmware/subgames/island/cloud2.bmp",
    },
    {
        .path = "/firmware/subgames/island/cloud3.bmp",
    }
};

#define OBJECTS_TYPES_COUNT TSGL_CALC_ARRSIZE(objects)
static tsgl_sprite* gameobj_sprites[OBJECTS_TYPES_COUNT];

typedef struct {
    bool gameover;
    time_t oldTimerTickTime;
    
    int score;
    int score_delta;

    tsgl_sound* music;

    Gameobj_state objs[MAX_OBJECTS];
} Subgame_state;

static Subgame_state* subgame_state = NULL;

static void obj_spawn(tsgl_pos x, tsgl_pos y, uint8_t type) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        if (subgame_state->objs[i].type < 0) {
            tsgl_sprite* sprite = gameobj_sprites[type];
            Gameobj_state* state = subgame_state->objs[i];

            //memset(state, 0, sizeof(Gameobj_state));
            state->x = x;
            state->y = y;
            state->type = type;
            state->sprite = sprite;
            return;
        }
    }
}

static void obj_destroy(size_t index) {
    subgame_state->objs[index].type = -1;
}

void subgame_island_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->music = pushsound_loop(music_path, MUSIC_SAMPLERATE, MUSIC_VOLUME);
    subgame_state->oldTimerTickTime = tsgl_time();
    subgame_state->score_delta = DEFAULT_SCORE_DELTA;

    for (size_t i = 0; i < OBJECTS_TYPES_COUNT; i++) {
        gameobj_sprites[i] = gfx_loadSprite(objects_settings[i].path);
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

    for (size_t i = 0; i < OBJECTS_TYPES_COUNT; i++) {
        tsgl_bmp_free(gameobj_sprites[i]);
        gameobj_sprites[i] = NULL;
    }

    free(subgame_state);
    subgame_state = NULL;

    game_alt_handle = NULL;
}

static void gameover() {
    stop_music();
    pushsound_play(sound_gameover_path, SOUND_GAMEOVER_SAMPLERATE, SOUND_GAMEOVER_VOLUME);
    subgame_state->gameover = true;
}

static void process() {
    obj_spawn(16, 16, 0);
    obj_spawn(16, 32, 1);
    obj_spawn(32, 32, 2);
    obj_spawn(32, 16, 3);
}

static void draw() {
}

void subgame_island_handle() {
    if (tsgl_keyboard_whenHold(&keyboard, KEY_INDEX_LEFT) && tsgl_keyboard_whenHold(&keyboard, KEY_INDEX_RIGHT)) {
        game_exit();
        return;
    }

    if (subgame_state->gameover) {
        game_modal_draw_gameover(subgame_state->score, current_state.subgame_island_max_score);
        return;
    }

    time_t currentTime = tsgl_time();
    if (currentTime - subgame_state->oldTimerTickTime > 1000) {
        subgame_state->oldTimerTickTime = currentTime;

        game_states_change(&current_state.states_sadness, SADNESS_DELTA * GAMECFG_PARAMS_SPEED_MUL);

        subgame_state->score += subgame_state->score_delta;
    }
    
    if (subgame_state->score > current_state.subgame_island_max_score)
        current_state.subgame_island_max_score = subgame_state->score;

    process();

    tsgl_framebuffer_clear(&framebuffer, BG_COLOR);

    draw();

    printsettings_subgames_line.fg = white;

    tsgl_pos draw_y = PRINT_START_POS_Y;
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "SCORE: %i", subgame_state->score);
    tsgl_framebuffer_text(&framebuffer, PRINT_START_POS_X, draw_y, printsettings_subgames_line, text);
    draw_y += PRINT_GAP_Y;

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "HIGH: %i", current_state.subgame_island_max_score);
    tsgl_framebuffer_text(&framebuffer, PRINT_START_POS_X, draw_y, printsettings_subgames_line, text);
    draw_y += PRINT_GAP_Y;
}

void subgame_island_exit() {
    game_exit();
}
