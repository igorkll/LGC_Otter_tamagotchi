#include "game_actions.h"
#include "game_upmenu.h"
#include "../pushsound.h"
#include "game_states.h"

void game_actions_sleep_withoutSound(int sleepTime) {
    if (current_state.sleepTimer > 0) return;
    current_state.sleepTimer = sleepTime;
    current_state.sleepStartTimer = sleepTime;
    game_sleepIn();
}

void game_actions_sleep(int sleepTime) {
    if (current_state.sleepTimer > 0) return;
    pushsound_play("/firmware/sounds/sadness.pcm", 16000, SADNESS_SOUND_VOLUME);
    game_actions_sleep_withoutSound(sleepTime);
}

void game_actions_eat() {
    if (current_state.backpack_eat_count <= 0) return;
    current_state.backpack_eat_count--;

    pushsound_play("/firmware/sounds/eat.pcm", 16000, 1);
    game_states_change(&current_state.states_hunger, -(100.0 / 2.0));
}

void game_actions_drink() {
    if (current_state.backpack_water_count <= 0) return;
    current_state.backpack_water_count--;
    
    pushsound_play("/firmware/sounds/drinking.pcm", 16000, 1);
    game_states_change(&current_state.states_thirst, -(100.0 / 3.0));
}

void game_actions_patPat() {
    pushsound_play("/firmware/sounds/question.pcm", 16000, QUESTION_SOUND_VOLUME);
    game_states_change(&current_state.states_caress, -(100.0 / 3.0));
    game_states_change(&current_state.states_sadness, -(100.0 / 7.0));
}
