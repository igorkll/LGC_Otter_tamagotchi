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

// ----------------------------------------------------------

typedef struct {
    bool gameover;
    time_t oldTimerTickTime;
    
    int score;
    int score_delta;

    tsgl_sound* music;
} Subgame_state;

static Subgame_state* subgame_state = NULL;

void subgame_island_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->music = pushsound_loop(music_path, MUSIC_SAMPLERATE, MUSIC_VOLUME);
    subgame_state->oldTimerTickTime = tsgl_time();
    subgame_state->score_delta = DEFAULT_SCORE_DELTA;
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

void subgame_island_handle() {
    if (tsgl_keyboard_getState(&keyboard, KEY_INDEX_CANCEL)) {
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

    tsgl_framebuffer_clear(&framebuffer, BG_COLOR);

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
