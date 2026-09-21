#include "racing.h"
#include "../gfx.h"
#include "../pushsound.h"
#include "../hctl.h"
#include "../game/game_modal.h"
#include "../game/game_printsets.h"

#define STATUS_ZONE 50
#define GAME_ZONE (WIDTH - STATUS_ZONE)
#define SEPARATOR_LINE_SIZE 2

#define COLOR_GRASS tsgl_color_raw(tsgl_color_fromHex(0x166633), framebuffer.colormode)
#define COLOR_ROAD_DOT tsgl_color_raw(tsgl_color_fromHex(0x777777), framebuffer.colormode)

#define START_CAR_POS_X (GAME_ZONE / 2)

#define PRINT_START_POS_Y 5
#define PRINT_GAP_Y 25

#define TAXIING_SPEED 2

#define ROAD_DOTS_COUNT 32
#define ROAD_DOT_SIZE 3

#define MAX_OBJECTS 8

#define SPEED_BOOST 3

typedef struct {
    const char* path;
    bool gameover;
    int score_delta;
    int fuel_delta;
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
        .gameover = true
    }
};

#define OBJECTS_TYPES_COUNT TSGL_CALC_ARRSIZE(objects)

static tsgl_sprite* gameobj_sprites[OBJECTS_TYPES_COUNT];

typedef int64_t global_pos;

typedef struct {
    bool okay_unlocked;
    bool gameover;

    tsgl_sprite* car_sprite;
    tsgl_sprite* person_sprite;
    tsgl_pos car_x;
    tsgl_pos car_y;
    tsgl_pos size_x;
    tsgl_pos size_y;
    
    int score;
    int fuel;
    tsgl_pos speed;
    global_pos scroll;

    tsgl_pos road_dots_x[ROAD_DOTS_COUNT];
    tsgl_pos road_dots_y[ROAD_DOTS_COUNT];

    Gameobj_state objs[MAX_OBJECTS];
} Subgame_state;

static Subgame_state* subgame_state = NULL;

void subgame_racing_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->speed = 5;
    subgame_state->fuel = 20;

    subgame_state->car_sprite = gfx_loadSprite("/firmware/subgames/racing/gamecar.bmp");
    subgame_state->person_sprite = game_getPersonSprite();

    subgame_state->size_x = subgame_state->car_sprite->sprite->width;
    subgame_state->size_y = subgame_state->car_sprite->sprite->height;

    subgame_state->car_x = GAME_ZONE / 2;
    subgame_state->car_y = HEIGHT - (subgame_state->size_y / 2) - 10;

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

static void game_exit() {
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
    pushsound_play("/firmware/sounds/gameover.pcm", 16000, EFFECTS_SOUND_VOLUME);

    subgame_state->gameover = true;
}

static void obj_collision(size_t index) {
    Gameobj_state* gameobj_state = &subgame_state->objs[index];
    if (gameobj_state->interacted) return;
    gameobj_state->interacted = true;

    Gameobj gameobj = objects[gameobj_state->type];

    if (gameobj.gameover) {
        gameover();
        return;
    }

    subgame_state->score += gameobj.score_delta;
    subgame_state->fuel += gameobj.fuel_delta;
}

static void spawn_random() {
    obj_spawn(tsgl_random(0, OBJECTS_TYPES_COUNT - 1));
}

static time_t oldTimerTickTime = -9999;
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

    time_t currentTime = tsgl_time();
    if (currentTime - oldTimerTickTime > 1000) {
        oldTimerTickTime = currentTime;

        subgame_state->fuel--;
        if (subgame_state->fuel < 0) {
            subgame_state->fuel = 0;
            gameover();
            return;
        }

        subgame_state->score++;

        spawn_random();
    }
    
    if (subgame_state->score > current_state.subgame_recing_max_score)
        current_state.subgame_recing_max_score = subgame_state->score;

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_LEFT)) { //LEFT
        subgame_state->car_x -= TAXIING_SPEED;

        tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
        if (car_x < 0) {
            subgame_state->car_x = subgame_state->size_x / 2;
        }
    }

    tsgl_pos speed = subgame_state->speed;
    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_OKAY) && subgame_state->okay_unlocked) { //BOOST
        speed += SPEED_BOOST;
    } else {
        subgame_state->okay_unlocked = true;
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
        game_exit();
        return;
    }

    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_RIGHT)) { //RIGHT
        subgame_state->car_x += TAXIING_SPEED;
        
        tsgl_pos car_x = subgame_state->car_x - (subgame_state->size_x / 2);
        if (car_x > (GAME_ZONE - subgame_state->size_x)) {
            subgame_state->car_x = GAME_ZONE - (subgame_state->size_x / 2);
        }
    }

    subgame_state->scroll += speed;

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
            subgame_state->road_dots_y[i] = -ROAD_DOT_SIZE;
        }

        tsgl_pos x = subgame_state->road_dots_x[i];
        tsgl_pos y = subgame_state->road_dots_y[i];
        tsgl_framebuffer_fill(&framebuffer, x, y, ROAD_DOT_SIZE, ROAD_DOT_SIZE, color_road_dot);
    }

    for (size_t i = 0; i < MAX_OBJECTS; i++) {
        if (subgame_state->objs[i].type < 0) continue;
        subgame_state->objs[i].y += speed;

        tsgl_sprite* sprite = subgame_state->objs[i].sprite;
        tsgl_framebuffer_push(&framebuffer, subgame_state->objs[i].x, subgame_state->objs[i].y, sprite);

        if (subgame_state->objs[i].y >= HEIGHT) {
            subgame_state->objs[i].type = -1;
        } else if (tsgl_funcs_checkIntersection(
            car_x, car_y, subgame_state->size_x, subgame_state->size_y,
            subgame_state->objs[i].x, subgame_state->objs[i].y, sprite->sprite->width, sprite->sprite->height
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
}
