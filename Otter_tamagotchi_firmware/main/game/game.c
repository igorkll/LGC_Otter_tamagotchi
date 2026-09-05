#include "game.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../pushsound.h"
#include "../funcs.h"
#include "game_upmenu.h"
#include "game_room_action.h"

// ------------------------------------ consts

static const char* game_state_path = "/storage/gamestat";

const Room rooms[] = {
    {
        .background = "bedroom",
        .music = NULL,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "kitchen",
        .music = NULL,
        .person_x = (WIDTH / 4) - 20,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "gaming",
        .music = NULL,
        .person_x = (WIDTH / 4) + 20,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "toilet",
        .music = NULL,
        .person_x = (WIDTH / 4) * 2,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "yard",
        .music = NULL,
        .person_x = WIDTH / 4,
        .person_y = (HEIGHT / 4) * 2,
    },
    {
        .background = "car",
        .music = "car",
        .musicVolume = 1,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "shop",
        .music = "shop",
        .musicVolume = 0.6,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    }
};

static const char* game_persons_images[] = {
    "otter"
};

#define ROOMS_COUNT TSGL_CALC_ARRSIZE(rooms)

static const Game_state default_state = {
    .room = game_room_bedroom,
    .person = game_person_otter
};

#define ACTION_TIMER_SIZE_MUL 0.8
#define ACTION_TIMER_LINE_PADDING 4 
#define ACTION_TIMER_HEIGHT 20
#define ACTION_TIMER_OBJ_OFFSET 12

#define ACTION_TIMER_TEXT_TARGET_WIDTH 10
#define ACTION_TIMER_TEXT_TARGET_HEIGHT 16

// ------------------------------------ vars

Game_state current_state;
Game_state old_state;
static tsgl_benchmark benchmark;
static tsgl_sprite* room_sprite = NULL;
static tsgl_sprite* person_sprite = NULL;
static tsgl_sound* room_music;

// ------------------------------------ functions

const Room* game_getCurrentRoom() {
    return &rooms[current_state.room];
}

size_t game_getCurrentRoomIndex() {
    return current_state.room;
}

bool game_isLockedInRoom() {
    return current_state.room >= ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT;
}

const char* game_getCurrentPerson() {
    return game_persons_images[current_state.person];
}

void game_save() {
    if (tsgl_filesystem_writeFile(game_state_path, &current_state, sizeof(Game_state)) == sizeof(Game_state)) {
        ESP_LOGI(TAG, "game saved");
    } else {
        ESP_LOGE(TAG, "failed to save game");
    }
}

static void game_load() {
    memcpy(&current_state, &default_state, sizeof(Game_state));
    if (tsgl_filesystem_exists(game_state_path)) {
        if (tsgl_filesystem_readFile(game_state_path, &current_state, sizeof(Game_state)) == sizeof(Game_state)) {
            ESP_LOGI(TAG, "game loaded");
        } else {
            ESP_LOGE(TAG, "failed to load game");
        }
    } else {
        ESP_LOGI(TAG, "game default loaded");
    }
    memcpy(&old_state, &current_state, sizeof(Game_state));
}

static void unload_room_sound() {
    if (room_music) {
        tsgl_sound_free(room_music);
        room_music = NULL;
    }
}

void game_selectRoom(int index) {
    current_state.room = index;
    if (index != ID_CAR) current_state.old_car_room = index;

    for (size_t i = 0; i < ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT; i++) {
        game_upmenu_setActivate(i, index == i);
    }

    unload_room_sound();

    const Room* room = game_getCurrentRoom();
    if (room->music != NULL) {
        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/firmware/music/%s.pcm", room->music);

        room_music = pushsound_loop(path, 4000, room->musicVolume);
        tsgl_sound_setLoop(room_music, true);
    }

    game_upmenu_redrawTitle();
    game_upmenu_reloadIcons();
}

// ------------------------------------ process

void game_sleepIn() {
    hctl_enableAutoBacklight(false);
    hctl_setBacklight(BACKLIGHT_IDLE);
}

static void sleepOut() {
    hctl_enableAutoBacklight(true);
    hctl_setBacklight(BACKLIGHT_MAX);
}

static void loadSprites() {
    static int room_sprite_old_index = 0;
    if (room_sprite == NULL || current_state.room != room_sprite_old_index) {
        room_sprite_old_index = current_state.room;
        if (room_sprite != NULL) tsgl_bmp_free(room_sprite);

        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/firmware/rooms/%s.bmp", game_getCurrentRoom()->background);
        room_sprite = gfx_loadSprite(path);
    }

    static int room_person_old_index = 0;
    if (person_sprite == NULL || current_state.person != room_person_old_index) {
        room_person_old_index = current_state.person;
        if (person_sprite != NULL) tsgl_bmp_free(person_sprite);

        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/firmware/persons/%s.bmp", game_getCurrentPerson());
        person_sprite = gfx_loadSprite(path);
    }
}

static void start() {
    if (current_state.sleepTimer > 0) {
        game_sleepIn();
    }

    game_selectRoom(current_state.room);
}

static void exit_myaaaa(tsgl_sound* sound) {
    bool* running = (bool*)sound->userData;
    *running = false;
}

static void run_myaaaa() {
    unload_room_sound();
    hctl_enableAutoBacklight(false);
    hctl_resetIdleTimer();
    hctl_setBacklight(BACKLIGHT_MAX);

    gfx_drawCenteredScreenImage("/firmware/myaaaa/myaaaa.bmp");
    tsgl_display_send(&display, &framebuffer);

    bool running = true;

    tsgl_sound* sound = pushsound_play("/firmware/myaaaa/myaaaa.pcm", 8000, MYAAAA_SOUND_VOLUME);
    sound->userData = (void*)&running;
    tsgl_sound_attachCallback_end(sound, exit_myaaaa);

    while (running) {
        tsgl_keyboard_readAll(&keyboard);

        if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_CANCEL)) {
            tsgl_sound_free(sound);
            running = false;
        }
    }

    hctl_enableAutoBacklight(true);
    hctl_resetIdleTimer();
    
    tsgl_benchmark_reset(&benchmark);
    game_selectRoom(current_state.room);
}

static void onTimerAction() {
    switch (current_state.actionTimer_action) {
        case game_action_switchRoom:
            game_selectRoom(current_state.actionTimer_nextRoom);
            break;
        
        default:
            break;
    }
}

static time_t oldTimerTickTime = -9999;
static void checkActionTimer() {
    time_t currentTime = tsgl_time();
    if (currentTime - oldTimerTickTime > 1000) {
        oldTimerTickTime = currentTime;

        if (current_state.actionTimer > 0) {
            current_state.actionTimer--;
            if (current_state.actionTimer <= 0) {
                onTimerAction();
                current_state.actionTimer = 0;
            }
        }

        if (current_state.sleepTimer > 0) {
            current_state.sleepTimer--;
            if (current_state.sleepTimer <= 0) {
                sleepOut();
                current_state.sleepTimer = 0;
            }
        }
    }
}

static void processControl() {
    if (current_state.sleepTimer == 0) {
        bool allPressed = true;
        for (size_t i = 0; i < KEYS_COUNT; i++) {
            if (!tsgl_keyboard_getState(&keyboard, i)) {
                allPressed = false;
                break;
            }
        }
        if (allPressed) {
            run_myaaaa();
        }
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_CANCEL)) {
        if (current_state.sleepTimer > 0) {
            sleepOut();
            current_state.sleepTimer = 0;
            return;
        }

        if (current_state.actionTimer > 0 && current_state.actionTimer_allowCancel) {
            game_stopActionTimer();
            return;
        }
    }
    
    if (current_state.sleepTimer == 0) {
        int used = game_upmenu_process();
        if (used >= 0) {
            if (used < ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT && !game_isLockedInRoom()) {
                game_selectRoom(used);
            } else {
                game_roomAction(used);
            }
        }
    }
}

static void process() {
    checkActionTimer();
    hctl_process();

    processControl();

    if (memcmp(&current_state, &old_state, sizeof(Game_state)) != 0) {
        old_state = current_state;
        game_save();
    }
}

static void drawPerson() {
    const Room* room = game_getCurrentRoom();
    gfx_drawCenteredImageSpriteWithTransparentSupport(room->person_x, room->person_y, person_sprite);
}

static tsgl_print_settings printsettings_actiontimer_title = {
    .multiline = true,
    .locationMode = tsgl_print_start_top,
    .height = ACTION_TIMER_HEIGHT,
    .globalAlignmentX = tsgl_print_alignment_center,
    .globalAlignmentY = tsgl_print_alignment_center,
    .alignment = tsgl_print_alignment_center,
    
    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = ACTION_TIMER_TEXT_TARGET_WIDTH,
    .targetHeight = ACTION_TIMER_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static void drawActionTimer() {
    if (current_state.actionTimer <= 0) return;

    tsgl_pos sizeX = WIDTH * ACTION_TIMER_SIZE_MUL;
    tsgl_pos sizeY = ACTION_TIMER_HEIGHT;
    tsgl_pos fillSize = tsgl_math_imap(current_state.actionTimer, current_state.actionTimer_max, 0, 0, sizeX - (ACTION_TIMER_LINE_PADDING * 2));
    
    tsgl_pos positionX = (WIDTH / 2) - (sizeX / 2);
    tsgl_pos positionY = (HEIGHT / 2) - (sizeY / 2);

    tsgl_pos positionY_line = positionY - ACTION_TIMER_OBJ_OFFSET;
    tsgl_pos positionY_text = positionY + ACTION_TIMER_OBJ_OFFSET;

    tsgl_framebuffer_rect(&framebuffer, positionX, positionY_line, sizeX, sizeY, red, 2);
    tsgl_framebuffer_fill(&framebuffer, positionX + ACTION_TIMER_LINE_PADDING, positionY_line + ACTION_TIMER_LINE_PADDING, fillSize, sizeY - (ACTION_TIMER_LINE_PADDING * 2), red);

    printsettings_actiontimer_title.width = sizeX;
    printsettings_actiontimer_title.fg = red;
    tsgl_framebuffer_text(&framebuffer, positionX, positionY_text, printsettings_actiontimer_title, current_state.actionTimer_str);
}

static void render() {
    if (current_state.sleepTimer > 0) {
        tsgl_framebuffer_clear(&framebuffer, black);
        return;
    }

    loadSprites();
    gfx_drawCenteredScreenImageSprite(room_sprite);
    drawPerson();
    drawActionTimer();
    game_upmenu_draw();
}

void game_start() {
    ESP_LOGI(TAG, "game started!");
    tsgl_benchmark_reset(&benchmark);
    game_load();

    game_upmenu_init();
    start();

    while (true) {
        process();

        tsgl_benchmark_startRendering(&benchmark);
        render();
        tsgl_benchmark_endRendering(&benchmark);

        tsgl_benchmark_startSend(&benchmark);
        tsgl_display_send(&display, &framebuffer);
        tsgl_benchmark_endSend(&benchmark);

        #ifdef DEBUG_FPS
            tsgl_benchmark_print(&benchmark);
        #endif
        tsgl_benchmark_wait(&benchmark, TARGET_FPS);
    }
}

void game_startActionTimer(int actionTimer, const char* str, game_action action, game_room nextRoom, bool allowCancel) {
    current_state.actionTimer = actionTimer;
    current_state.actionTimer_max = actionTimer;
    slnprintf(current_state.actionTimer_str, MAX_ACTION_LEN, "%s", str);
    current_state.actionTimer_action = action;
    current_state.actionTimer_nextRoom = nextRoom;
    current_state.actionTimer_allowCancel = allowCancel;
}

void game_stopActionTimer() {
    current_state.actionTimer = 0;
}
