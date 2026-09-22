#include "racing.h"
#include "../gfx.h"
#include "../pushsound.h"
#include "../hctl.h"
#include "../game/game_modal.h"
#include "../game/game_printsets.h"
#include "../game/game_states.h"

#define STATUS_ZONE 50
#define GAME_ZONE (WIDTH - STATUS_ZONE)
#define SEPARATOR_LINE_SIZE 2

#define COLOR_GRASS tsgl_color_raw(tsgl_color_fromHex(0x166633), framebuffer.colormode)
#define COLOR_ROAD_DOT tsgl_color_raw(tsgl_color_fromHex(0x777777), framebuffer.colormode)

#define START_CAR_POS_X (GAME_ZONE / 2)

#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 25

#define ROAD_DOTS_COUNT 32
#define ROAD_DOT_SIZE 3

#define MAX_OBJECTS 8

#define SPEED_BOOST_FUEL_DELTA 4

#define DEFAULT_SPEED 2
#define DEFAULT_FUEL 60
#define DEFAULT_SPAWN_OBJECT_PER_SCROLL 30
#define DEFAULT_SCORE_DELTA 1
#define DEFAULT_SPAWN_PERCENT 50
#define DEFAULT_TAXIING_SPEED 2
#define DEFAULT_SPEED_BOOST 3
#define DEFAULT_SPEED_BOOST_TAXIING_SPEED_ADD 5

#define MUSIC_CHANGE_SPEED_FACTOR 0.1

static const char* music_path = "/firmware/music/edmvselo.dpw";
#define MUSIC_SAMPLERATE 16000
#define MUSIC_VOLUME 0.6

#define FATIGUE_DELTA 0.05
#define SADNESS_DELTA -1.1

typedef struct {
    const char* path;
    bool gameover;
    bool delete;
    int self_speed;
    int score_delta;
    int score_delta_delta;
    int speed_delta;
    int taxiing_speed_delta;
    int fuel_delta;
    int speed_boost_delta;
    int speed_boost_taxiing_speed_add;
} Gameobj;

typedef struct {
    tsgl_pos x;
    tsgl_pos y;
    int8_t type;
    tsgl_sprite* sprite;
    bool interacted;
} Gameobj_state;

static const Gameobj objects[] = {
    {
        .path = "/firmware/subgames/racing/stone0.bmp",
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/stone1.bmp",
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/stone2.bmp",
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/enemycar.bmp",
        .self_speed = 3,
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/fuel.bmp",
        .fuel_delta = 60,
        .score_delta = 10,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/wheel.bmp",
        .taxiing_speed_delta = 1,
        .score_delta = 10,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/engine.bmp",
        .speed_delta = 1,
        .score_delta = 10,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/truster.bmp",
        .score_delta = 10,
        .speed_boost_delta = 1,
        .speed_boost_taxiing_speed_add = 1,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/star.bmp",
        .score_delta = 50,
        .delete = true
    }
};

#define OBJECTS_TYPES_COUNT TSGL_CALC_ARRSIZE(objects)

static tsgl_sprite* gameobj_sprites[OBJECTS_TYPES_COUNT];

typedef int64_t global_pos;

typedef struct {
    bool okay_unlocked;
    bool gameover;
    bool old_boost;

    tsgl_sprite* car_sprite;
    tsgl_sprite* person_sprite;
    tsgl_pos car_x;
    tsgl_pos car_y;
    tsgl_pos size_x;
    tsgl_pos size_y;
    
    int score;
    int score_delta;
    int taxiing_speed;
    int8_t spawn_percent;
    int fuel;
    tsgl_pos speed;
    global_pos scroll;

    int speed_boost;
    int speed_boost_taxiing_speed_add;

    global_pos old_spawn_scroll;
    global_pos spawn_object_per_scroll;

    tsgl_pos road_dots_x[ROAD_DOTS_COUNT];
    tsgl_pos road_dots_y[ROAD_DOTS_COUNT];

    Gameobj_state objs[MAX_OBJECTS];

    time_t oldTimerTickTime;

    tsgl_sound* music;
} Subgame_state;

static Subgame_state* subgame_state = NULL;

void subgame_racing_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->speed = DEFAULT_SPEED;
    subgame_state->fuel = DEFAULT_FUEL;

    subgame_state->car_sprite = gfx_loadSprite("/firmware/subgames/racing/gamecar.bmp");
    subgame_state->person_sprite = game_getPersonSprite();

    subgame_state->size_x = subgame_state->car_sprite->sprite->width;
    subgame_state->size_y = subgame_state->car_sprite->sprite->height;

    subgame_state->car_x = GAME_ZONE / 2;
    subgame_state->car_y = HEIGHT - (subgame_state->size_y / 2) - 10;

    subgame_state->oldTimerTickTime = tsgl_time();

    subgame_state->spawn_object_per_scroll = DEFAULT_SPAWN_OBJECT_PER_SCROLL;
    subgame_state->score_delta = DEFAULT_SCORE_DELTA;
    subgame_state->spawn_percent = DEFAULT_SPAWN_PERCENT;
    subgame_state->taxiing_speed = DEFAULT_TAXIING_SPEED;

    subgame_state->speed_boost = DEFAULT_SPEED_BOOST;
    subgame_state->speed_boost_taxiing_speed_add = DEFAULT_SPEED_BOOST_TAXIING_SPEED_ADD;

    subgame_state->music = pushsound_loop(music_path, MUSIC_SAMPLERATE, MUSIC_VOLUME);

    for (size_t i = 0; i < ROAD_DOTS_COUNT; i++) {
        subgame_state->road_dots_x[i] = tsgl_random(0, GAME_ZONE - ROAD_DOT_SIZE);
        subgame_state->road_dots_y[i] = tsgl_random(-ROAD_DOT_SIZE, HEIGHT);
    }

    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        subgame_state->objs[i].type = -1;
    }

    for (size_t i = 0; i < OBJECTS_TYPES_COUNT; i++) {
        gameobj_sprites[i] = gfx_loadSprite(objects[i].path);
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

    tsgl_bmp_free(subgame_state->car_sprite);
    subgame_state->car_sprite = NULL;

    free(subgame_state);
    subgame_state = NULL;

    game_alt_handle = NULL;
}

static void obj_spawn(uint8_t type) {
    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        if (subgame_state->objs[i].type < 0) {
            tsgl_sprite* sprite = gameobj_sprites[type];

            subgame_state->objs[i].type = type;
            subgame_state->objs[i].sprite = sprite;
            subgame_state->objs[i].x = tsgl_random(0, GAME_ZONE - sprite->sprite->width);
            subgame_state->objs[i].y = -sprite->sprite->height;
            subgame_state->objs[i].interacted = false;
            return;
        }
    }
}

static void gameover() {
    stop_music();
    pushsound_play("/firmware/sounds/gameover.pcm", 16000, 1);
    subgame_state->gameover = true;
}

static void obj_delete(size_t index) {
    subgame_state->objs[index].type = -1;
}

static void obj_collision(size_t index) {
    Gameobj_state* gameobj_state = &subgame_state->objs[index];
    if (gameobj_state->interacted) return;
    gameobj_state->interacted = true;

    Gameobj gameobj = objects[gameobj_state->type];

    if (gameobj.gameover) {
        gameover();
    }

    subgame_state->score += gameobj.score_delta;
    subgame_state->score_delta += gameobj.score_delta_delta;
    subgame_state->speed += gameobj.speed_delta;
    subgame_state->taxiing_speed += gameobj.taxiing_speed_delta;
    subgame_state->fuel += gameobj.fuel_delta;

    if (gameobj.delete) obj_delete(index);
}

static void spawn_random() {
    if (subgame_state->scroll - subgame_state->old_spawn_scroll > subgame_state->spawn_object_per_scroll) {
        subgame_state->old_spawn_scroll = subgame_state->scroll;
        if (tsgl_random(0, 99) < subgame_state->spawn_percent) {
            obj_spawn(tsgl_random(0, OBJECTS_TYPES_COUNT - 1));
        }
    }
}

void subgame_racing_handle() {
    // ------------------------ process

    if (subgame_state->gameover) {
        if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
            game_exit();
            return;
        }

        game_modal_draw_gameover(subgame_state->score, current_state.subgame_recing_max_score);

        return;
    }

    bool boost = false;
    tsgl_pos speed = subgame_state->speed;
    tsgl_pos add_taxiing_speed = 0;
    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_OKAY)) { //BOOST
        if (subgame_state->okay_unlocked) {
            speed += subgame_state->speed_boost;
            add_taxiing_speed = subgame_state->speed_boost_taxiing_speed_add;
            boost = true;
            if (!subgame_state->old_boost) {
                subgame_state->fuel -= SPEED_BOOST_FUEL_DELTA;
            }
        }
    } else {
        subgame_state->okay_unlocked = true;
    }
    subgame_state->old_boost = boost;

    time_t currentTime = tsgl_time();
    if (currentTime - subgame_state->oldTimerTickTime > 1000) {
        subgame_state->oldTimerTickTime = currentTime;

        game_states_change(&current_state.states_fatigue, FATIGUE_DELTA);
        game_states_change(&current_state.states_sadness, SADNESS_DELTA);

        subgame_state->fuel--;

        if (boost) {
            subgame_state->fuel -= SPEED_BOOST_FUEL_DELTA;
        }

        if (subgame_state->fuel < 0) {
            subgame_state->fuel = 0;
        } else {
            subgame_state->score += subgame_state->score_delta;
        }
    }
    
    if (subgame_state->score > current_state.subgame_recing_max_score)
        current_state.subgame_recing_max_score = subgame_state->score;

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_LEFT)) { //LEFT
        subgame_state->car_x -= subgame_state->taxiing_speed + add_taxiing_speed;

        tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
        if (car_x < 0) {
            subgame_state->car_x = subgame_state->size_x / 2;
        }
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
        game_exit();
        return;
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_RIGHT)) { //RIGHT
        subgame_state->car_x += subgame_state->taxiing_speed + add_taxiing_speed;
        
        tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
        if (car_x > (GAME_ZONE - subgame_state->size_x)) {
            subgame_state->car_x = GAME_ZONE - (subgame_state->size_x / 2);
        }
    }

    subgame_state->scroll += speed;
    
    float music_speed = (((((float)speed) / ((float)DEFAULT_SPEED)) - 1.0) * MUSIC_CHANGE_SPEED_FACTOR) + 1.0;
    if (subgame_state->music->speed != music_speed) {
        tsgl_sound_setSpeed(subgame_state->music, music_speed);
    }

    spawn_random();

    // ------------------------ draw

    tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
    tsgl_pos car_y = subgame_state->car_y - (subgame_state->size_y / 2);

    tsgl_framebuffer_fill(&framebuffer, 0, 0, GAME_ZONE, HEIGHT, COLOR_GRASS);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, STATUS_ZONE, HEIGHT, black);
    tsgl_framebuffer_fill(&framebuffer, GAME_ZONE, 0, SEPARATOR_LINE_SIZE, HEIGHT, white);

    tsgl_rawcolor color_road_dot = COLOR_ROAD_DOT;
    for (size_t i = 0; i < ROAD_DOTS_COUNT; i++) {
        subgame_state->road_dots_y[i] += speed;
        if (subgame_state->road_dots_y[i] >= HEIGHT) {
            subgame_state->road_dots_x[i] = tsgl_random(0, GAME_ZONE - ROAD_DOT_SIZE);
            subgame_state->road_dots_y[i] = tsgl_random(-(ROAD_DOT_SIZE * 2), -ROAD_DOT_SIZE);
        }

        tsgl_pos x = subgame_state->road_dots_x[i];
        tsgl_pos y = subgame_state->road_dots_y[i];
        tsgl_framebuffer_fill(&framebuffer, x, y, ROAD_DOT_SIZE, ROAD_DOT_SIZE, color_road_dot);
    }

    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        Gameobj_state* gameobj_state = &subgame_state->objs[i];
        if (gameobj_state->type < 0) continue;
        const Gameobj* gameobj = &objects[gameobj_state->type];

        gameobj_state->y += speed + gameobj->self_speed;

        tsgl_sprite* sprite = gameobj_state->sprite;
        tsgl_framebuffer_push(&framebuffer, gameobj_state->x, gameobj_state->y, sprite);

        if (gameobj_state->y >= HEIGHT) {
            gameobj_state->type = -1;
        } else if (tsgl_funcs_checkIntersection(
            car_x, car_y, subgame_state->size_x, subgame_state->size_y,
            gameobj_state->x, gameobj_state->y, sprite->sprite->width, sprite->sprite->height
        )) {
            obj_collision(i);
        }
    }

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

    TSGL_funcs_slnprintf(text, MAX_ACTION_LEN, "FUEL\n%i", subgame_state->fuel);
    tsgl_framebuffer_text(&framebuffer, GAME_ZONE, draw_y, printsettings_subgames, text);
    draw_y += PRINT_GAP_Y;

    PUSH_FUNC_TRANS(&framebuffer,
        (GAME_ZONE + (STATUS_ZONE / 2)) - (subgame_state->person_sprite->sprite->width / 2),
        HEIGHT - subgame_state->person_sprite->sprite->height - 2,
        subgame_state->person_sprite
    );

    if (car_x >= 0 && car_y >= 0
        && car_x <= (GAME_ZONE - subgame_state->size_x)
        && car_y <= (HEIGHT - subgame_state->size_y))
        PUSH_FUNC_TRANS(&framebuffer, car_x, car_y, subgame_state->car_sprite);

    if (subgame_state->fuel <= 0) {
        gameover();
    }
}
