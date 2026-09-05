#include "game_actions.h"
#include "../pushsound.h"

void game_actions_sleep(int sleepTime) {
    pushsound_play("/firmware/sounds/sadness.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.sleepTimer = sleepTime;
    game_sleepIn();
}