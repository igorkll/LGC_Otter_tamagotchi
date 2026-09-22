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

// ----------------------------------------------------------

typedef struct {
    bool gameover;
    time_t oldTimerTickTime;
    
    int score;
    int score_delta;

    tsgl_sound* music;
    tsgl_sprite* person_sprite;
} Subgame_state;

static Subgame_state* subgame_state = NULL;

void subgame_tetris_start() {
    subgame_state = calloc(1, sizeof(Subgame_state));

    subgame_state->music = pushsound_loop(music_path, MUSIC_SAMPLERATE, MUSIC_VOLUME);
    subgame_state->person_sprite = game_getPersonSprite();
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

    PUSH_FUNC_TRANS(&framebuffer,
        (GAME_ZONE + (STATUS_ZONE / 2)) - (subgame_state->person_sprite->sprite->width / 2),
        HEIGHT - subgame_state->person_sprite->sprite->height - 2,
        subgame_state->person_sprite
    );
}
