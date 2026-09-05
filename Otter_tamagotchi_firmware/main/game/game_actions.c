#include "game_actions.h"
#include "../pushsound.h"

void game_actions_sleep(int sleepTime) {
    pushsound_play("/firmware/sounds/sadness.pcm", 16000, 1);
    current_state.sleepTimer = sleepTime;
}