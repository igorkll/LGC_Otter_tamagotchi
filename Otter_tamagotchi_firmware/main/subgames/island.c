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
#define PRINT_GAP_Y 12

#define MAX_OBJECTS 32
#define MAX_CLOUDS_COUNT 8

#define MIN_CLOUD_SPAWN_POS_Y 0
#define MAX_CLOUD_SPAWN_POS_Y 32

#define RNDMAX_CLOUD_SPAWN 60

#define PLAYER_VELOCITY_CHANGE_X 0.5
#define PLAYER_VELOCITY_MAX_X 2
#define PLAYER_VELOCITY_DROP_X 0.8

// ----------------------------------------------------------

typedef enum {
    gameobj_setup_type_cloud = 0,
    gameobj_setup_type_brick
} Gameobj_setup_type;

typedef struct {
    const char* path;
    Gameobj_setup_type gameobj_setup_type;
    tsgl_pos delta_x;
    tsgl_pos delta_y;
} Gameobj_setup;

typedef struct {
    tsgl_pos x;
    tsgl_pos y;
    tsgl_pos screen_x;
    tsgl_pos screen_y;
    int8_t type;
    tsgl_sprite* sprite;
} Gameobj_state;

#include "cparts/island_objects.h"

#define OBJECTS_TYPES_COUNT TSGL_CALC_ARRSIZE(objects_settings)
static tsgl_sprite* gameobj_sprites[OBJECTS_TYPES_COUNT];

typedef struct {
    bool gameover;
    time_t oldTimerTickTime;
    
    int score;
    int score_delta;

    float player_x;
    float player_y;

    float player_vel_x;
    float player_vel_y;

    tsgl_sound* music;

    Gameobj_state objs[MAX_OBJECTS];
} Subgame_state;

static Subgame_state* subgame_state = NULL;

static int obj_spawn(tsgl_pos x, tsgl_pos y, uint8_t type) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        if (subgame_state->objs[i].type < 0) {
            tsgl_sprite* sprite = gameobj_sprites[type];
            Gameobj_state* state = &subgame_state->objs[i];

            //memset(state, 0, sizeof(Gameobj_state));
            state->x = x;
            state->y = y;
            state->type = type;
            state->sprite = sprite;
            return i;
        }
    }

    return -1;
}

static void obj_destroy(size_t index) {
    subgame_state->objs[index].type = -1;
}

static void gameStart();

void subgame_island_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->music = pushsound_loop(music_path, MUSIC_SAMPLERATE, MUSIC_VOLUME);
    subgame_state->oldTimerTickTime = tsgl_time();
    subgame_state->score_delta = DEFAULT_SCORE_DELTA;

    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        subgame_state->objs[i].type = -1;
    }

    for (size_t i = 0; i < OBJECTS_TYPES_COUNT; i++) {
        gameobj_sprites[i] = gfx_loadSprite(objects_settings[i].path);
    }

    gameStart();
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
        tsgl_sprite_free(gameobj_sprites[i]);
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

static int get_objects_with_type_count(Gameobj_setup_type game_setup_type) {
    int count = 0;
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        Gameobj_state* gameobj_state = &subgame_state->objs[i];
        if (gameobj_state->type < 0) continue;
        const Gameobj_setup* gameobj_setup = &objects_settings[gameobj_state->type];

        if (gameobj_setup->gameobj_setup_type == game_setup_type) count++;
    }
    return count;
}

static int get_registred_with_type_count(Gameobj_setup_type game_setup_type) {
    int count = 0;
    for (size_t i = 0; i < OBJECTS_TYPES_COUNT; i++) {
        const Gameobj_setup* gameobj_setup = &objects_settings[i];
        if (gameobj_setup->gameobj_setup_type == game_setup_type) count++;
    }
    return count;
}

static int8_t get_random_object_with_type(Gameobj_setup_type game_setup_type) {
    int type_count = get_registred_with_type_count(game_setup_type);
    if (type_count == 0) return -1;

    int random_idx = tsgl_random(0, type_count - 1);
    int current_idx = 0;

    for (size_t i = 0; i < OBJECTS_TYPES_COUNT; i++) {
        const Gameobj_setup* gameobj_setup = &objects_settings[i];

        if (gameobj_setup->gameobj_setup_type == game_setup_type) {
            if (current_idx == random_idx) {
                return i;
            }
            current_idx++;
        }
    }

    return -1;
}

static tsgl_pos get_object_width(int8_t type) {
    return gameobj_sprites[type]->fb->width;
}

static tsgl_pos get_object_height(int8_t type) {
    return gameobj_sprites[type]->fb->height;
}

static tsgl_pos globalPosToScreenPosX(tsgl_pos global_pos) {
    return global_pos - subgame_state->player_x;
}

static tsgl_pos globalPosToScreenPosY(tsgl_pos global_pos) {
    return global_pos - subgame_state->player_y;
}

static tsgl_pos screenPosToGlobalPosX(tsgl_pos screen_pos) {
    return subgame_state->player_x + screen_pos;
}

static tsgl_pos screenPosToGlobalPosY(tsgl_pos screen_pos) {
    return subgame_state->player_y + screen_pos;
}

static void spawn_random_cloud() {
    obj_spawn(screenPosToGlobalPosX(WIDTH), tsgl_random(MIN_CLOUD_SPAWN_POS_Y, MAX_CLOUD_SPAWN_POS_Y), get_random_object_with_type(gameobj_setup_type_cloud));
}

static void spawn_clouds() {
    if (tsgl_random(0, RNDMAX_CLOUD_SPAWN) == 0) {
        int clouds_count = get_objects_with_type_count(gameobj_setup_type_cloud);
        if (clouds_count < MAX_CLOUDS_COUNT) {
            spawn_random_cloud();
        }
    }
}

static void spawn_random_objects() {
    spawn_clouds();
}

static void process_objects() {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        Gameobj_state* gameobj_state = &subgame_state->objs[i];
        if (gameobj_state->type < 0) continue;
        const Gameobj_setup* gameobj_setup = &objects_settings[gameobj_state->type];

        gameobj_state->x += gameobj_setup->delta_x;
        gameobj_state->y += gameobj_setup->delta_y;

        gameobj_state->screen_x = globalPosToScreenPosX(gameobj_state->x);
        gameobj_state->screen_y = globalPosToScreenPosY(gameobj_state->y);
    }
}

static void gameStart() {
    int8_t type = get_random_object_with_type(gameobj_setup_type_brick);

    tsgl_pos width = get_object_width(type);
    tsgl_pos height = get_object_height(type);

    obj_spawn(screenPosToGlobalPosX(WIDTH / 2), screenPosToGlobalPosX(HEIGHT) - height, type);
}

static void process() {
    subgame_state->player_vel_x *= PLAYER_VELOCITY_DROP_X;

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_LEFT)) {
        subgame_state->player_vel_x -= PLAYER_VELOCITY_CHANGE_X;
        if (subgame_state->player_vel_x < -PLAYER_VELOCITY_MAX_X) subgame_state->player_vel_x = -PLAYER_VELOCITY_MAX_X;
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_RIGHT)) {
        subgame_state->player_vel_x += PLAYER_VELOCITY_CHANGE_X;
        if (subgame_state->player_vel_x > PLAYER_VELOCITY_MAX_X) subgame_state->player_vel_x = PLAYER_VELOCITY_MAX_X;
    }

    subgame_state->player_x += subgame_state->player_vel_x;
    subgame_state->player_y += subgame_state->player_vel_y;

    spawn_random_objects();
    process_objects();
}

static void draw() {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        Gameobj_state* gameobj_state = &subgame_state->objs[i];
        if (gameobj_state->type < 0) continue;
        tsgl_sprite* sprite = gameobj_state->sprite;
        tsgl_framebuffer_push(&framebuffer, gameobj_state->screen_x, gameobj_state->screen_y, sprite);
    }
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

        game_states_change(&current_state.states_sadness, SADNESS_DELTA);

        subgame_state->score += subgame_state->score_delta;
    }
    
    if (subgame_state->score > current_state.subgame_island_max_score)
        current_state.subgame_island_max_score = subgame_state->score;

    process();
    tsgl_framebuffer_clear(&framebuffer, BG_COLOR);
    draw();

    printsettings_subgames_line_stroke.fg = white;
    printsettings_subgames_line_stroke.stroke = black;

    tsgl_pos draw_y = PRINT_START_POS_Y;
    
    char text[MAX_ACTION_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "SCORE: %i", subgame_state->score);
    tsgl_framebuffer_text(&framebuffer, PRINT_START_POS_X, draw_y, printsettings_subgames_line_stroke, text);
    draw_y += PRINT_GAP_Y;

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "HIGH: %i", current_state.subgame_island_max_score);
    tsgl_framebuffer_text(&framebuffer, PRINT_START_POS_X, draw_y, printsettings_subgames_line_stroke, text);
    draw_y += PRINT_GAP_Y;
}

void subgame_island_exit() {
    game_exit();
}
