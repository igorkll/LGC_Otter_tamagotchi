#include "game_actions.h"
#include "../pushsound.h"

void game_actions_sleep(int sleepTime) {
    pushsound_play("/firmware/sounds/trigger.pcm", 8000, 1);
    current_state.sleepTimer = sleepTime;
}