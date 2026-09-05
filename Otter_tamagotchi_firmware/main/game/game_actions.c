#include "game_actions.h"
#include "../pushsound.h"

void game_actions_sleep(int sleepTime) {
    pushsound_play("/firmware/trigger.pcm");
    current_state.sleepTimer = sleepTime;
}