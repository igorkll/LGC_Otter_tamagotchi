#include "game_actions.h"
#include "game_upmenu.h"
#include "../pushsound.h"

void game_actions_sleep(int sleepTime) {
    pushsound_play("/firmware/sounds/sadness.pcm", 16000, EFFECTS_SOUND_VOLUME * 5);
    current_state.sleepTimer = sleepTime;
    current_state.sleepStartTimer = sleepTime;
    game_sleepIn();
}

void game_actions_eat() {
    pushsound_play("/firmware/sounds/eat.pcm", 16000, EFFECTS_SOUND_VOLUME * 2);
    
}

void game_actions_drink() {
    pushsound_play("/firmware/sounds/drinking.pcm", 16000, EFFECTS_SOUND_VOLUME * 2);
    
}
