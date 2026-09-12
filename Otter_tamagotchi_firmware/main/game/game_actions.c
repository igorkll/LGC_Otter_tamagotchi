#include "game_actions.h"
#include "game_upmenu.h"
#include "../pushsound.h"
#include "game_states.h"

void game_actions_sleep(int sleepTime) {
    pushsound_play("/firmware/sounds/sadness.pcm", 16000, EFFECTS_SOUND_VOLUME * SADNESS_SOUND_VOLUME);
    current_state.sleepTimer = sleepTime;
    current_state.sleepStartTimer = sleepTime;
    game_sleepIn();
}

void game_actions_eat() {
    pushsound_play("/firmware/sounds/eat.pcm", 16000, EFFECTS_SOUND_VOLUME);
    game_states_change(&current_state.states_hunger, -(100.0 / 2.0));
}

void game_actions_drink() {
    pushsound_play("/firmware/sounds/drinking.pcm", 16000, EFFECTS_SOUND_VOLUME);
    game_states_change(&current_state.states_thirst, -(100.0 / 3.0));
}
