#include "pushsound.h"

static tsgl_sound sounds[MAX_SOUNDS_COUNT] = {0};
static uint8_t current_sound_index = 0;

tsgl_sound* pushsound_getFreeSlot() {
    tsgl_sound* current_sound = &sounds[current_sound_index];

    int iterationLimit = MAX_SOUNDS_COUNT;
    while (current_sound->buffer) {
        current_sound_index++;
        if (current_sound_index >= MAX_SOUNDS_COUNT) current_sound_index = 0;
        current_sound = &sounds[current_sound_index];

        iterationLimit--;
        if (iterationLimit <= 0) break;
    }

    if (current_sound->buffer) {
        iterationLimit = MAX_SOUNDS_COUNT;
        while (current_sound->loop && current_sound->buffer) {
            current_sound_index++;
            if (current_sound_index >= MAX_SOUNDS_COUNT) current_sound_index = 0;
            current_sound = &sounds[current_sound_index];

            iterationLimit--;
            if (iterationLimit <= 0) break;
        }
        
        if (current_sound->buffer) tsgl_sound_free(current_sound);
    }

    return current_sound;
}

void pushsound_incrementSlot() {
    current_sound_index++;
    if (current_sound_index >= MAX_SOUNDS_COUNT) current_sound_index = 0;
}

tsgl_sound* pushsound_load(const char* path, int sample_rate) {
    tsgl_sound* current_sound = pushsound_getFreeSlot();

    if (tsgl_sound_load_pcmEx(current_sound, SOUND_BUFFER_SIZE, 0, path, sample_rate, 1, 1, tsgl_sound_pcm_unsigned, true) != ESP_OK) {
        return NULL;
    }
    
    pushsound_incrementSlot();

    return current_sound;
}

tsgl_sound* pushsound_play(const char* path, int sample_rate) {
    tsgl_sound* current_sound = pushsound_load(path, sample_rate);

    tsgl_sound_output* sound_outputs[] = {sound_output};
    tsgl_sound_enableFreeOnEnd(current_sound, true);
    tsgl_sound_setOutputs(current_sound, sound_outputs, 1, false);
    tsgl_sound_setVolume(current_sound, VOLUME_MUL);
    tsgl_sound_play(current_sound);

    return current_sound;
}