#include "pushsound.h"

static float master_volume = 0;
static float effect_volume = 0;
static float music_volume = 0;

static float calc_effect_volume(float user_volume) {
    return VOLUME_MUL * EFFECTS_SOUND_VOLUME * user_volume * master_volume * effect_volume;
}

static float calc_music_volume(float user_volume) {
    return VOLUME_MUL * MUSIC_SOUND_VOLUME * user_volume * master_volume * music_volume;
}

// ----------------------------------------- sound

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

// ----------------------------------------- nbs

static tsgl_nbs_loadedSamples* nbs_loadedSamples = NULL;

void pushsound_initNbs() {
    if (nbs_loadedSamples != NULL) return;
    
    nbs_loadedSamples = tsgl_nbs_loadSamples(NBS_SAMPLES_COUNT, NBS_SAMPLES_PREFIX, NBS_SAMPLES_SUFFIX, NBS_SAMPLES_SAMPLERATE, NBS_SAMPLES_BITRATE, NBS_SAMPLES_CHANNELS, NBS_SAMPLES_PCMFORMAT);
}

static tsgl_nbs* nbs_sounds[MAX_NBS_COUNT] = {0};
static uint8_t current_nbs_index = 0;

size_t pushsound_nbs_getFreeSlot() {
    tsgl_nbs* current_sound = nbs_sounds[current_nbs_index];

    int iterationLimit = MAX_NBS_COUNT;
    while (current_sound && current_sound->playing) {
        current_sound_index++;
        if (current_sound_index >= MAX_NBS_COUNT) current_sound_index = 0;
        current_sound = nbs_sounds[current_sound_index];

        iterationLimit--;
        if (iterationLimit <= 0) break;
    }

    if (current_sound && current_sound->playing) {
        iterationLimit = MAX_NBS_COUNT;
        while (current_sound && current_sound->playing && current_sound->loop) {
            current_sound_index++;
            if (current_sound_index >= MAX_NBS_COUNT) current_sound_index = 0;
            current_sound = nbs_sounds[current_sound_index];

            iterationLimit--;
            if (iterationLimit <= 0) break;
        }
        
        if (current_sound) tsgl_nbs_free(current_sound);
    }

    return current_sound_index;
}

void pushsound_nbs_incrementSlot() {
    current_sound_index++;
    if (current_sound_index >= MAX_SOUNDS_COUNT) current_sound_index = 0;
}

// ----------------------------------------- push sound

tsgl_sound* pushsound_load(const char* path, int sample_rate) {
    tsgl_sound* current_sound = pushsound_getFreeSlot();

    if (tsgl_sound_load_pcmEx(current_sound, SOUND_BUFFER_SIZE, 0, path,
            sample_rate, 1, 1,
            tsgl_sound_pcm_unsigned,
            USE_SOUND_DOUBLE_BUFFER) != ESP_OK)
        return NULL;

    if (TSGL_funcs_hasext(path, ".dpw")) {
        tsgl_sound_allocatePcmDecoder(current_sound, 0);
    }

    pushsound_incrementSlot();
    return current_sound;
}

tsgl_sound* pushsound_play(const char* path, int sample_rate, float volume) {
    tsgl_sound* current_sound = pushsound_load(path, sample_rate);

    tsgl_sound_output* sound_outputs[] = {sound_output};
    tsgl_sound_enableFreeOnEnd(current_sound, true);
    tsgl_sound_setOutputs(current_sound, sound_outputs, 1, false);
    tsgl_sound_setVolume(current_sound, calc_effect_volume(volume));
    tsgl_sound_play(current_sound);
    current_sound->userData_float = volume;
    current_sound->userData_int = 0;

    return current_sound;
}

tsgl_sound* pushsound_loop(const char* path, int sample_rate, float volume) {
    tsgl_sound* current_sound = pushsound_load(path, sample_rate);

    tsgl_sound_output* sound_outputs[] = {sound_output};
    tsgl_sound_setLoop(current_sound, true);
    tsgl_sound_setOutputs(current_sound, sound_outputs, 1, false);
    tsgl_sound_setVolume(current_sound, calc_music_volume(volume));
    tsgl_sound_play(current_sound);
    current_sound->userData_float = volume;
    current_sound->userData_int = 1;

    return current_sound;
}

// ----------------------------------------- push nbs

tsgl_nbs* pushsound_nbs_load(const char* path) {
    size_t free_slot = pushsound_nbs_getFreeSlot();
    tsgl_nbs* nbs = tsgl_nbs_load(nbs_loadedSamples, path, NBS_MAX_NOTES_COUNT);
    nbs_sounds[free_slot] = nbs;

    pushsound_nbs_incrementSlot();
    return nbs;
}

tsgl_nbs* pushsound_nbs_play(const char* path, float volume) {
    tsgl_nbs* current_sound = pushsound_nbs_load(path);

    tsgl_sound_output* sound_outputs[] = {sound_output};
    tsgl_nbs_setOutputs(current_sound, sound_outputs, 1);
    tsgl_nbs_setVolume(current_sound, calc_effect_volume(volume));
    tsgl_nbs_play(current_sound);
    current_sound->userData_float = volume;
    current_sound->userData_int = 0;

    return current_sound;
}

tsgl_nbs* pushsound_nbs_loop(const char* path, float volume) {
    tsgl_nbs* current_sound = pushsound_nbs_load(path);

    tsgl_sound_output* sound_outputs[] = {sound_output};
    tsgl_nbs_setOutputs(current_sound, sound_outputs, 1);
    tsgl_nbs_setVolume(current_sound, calc_music_volume(volume));
    tsgl_nbs_play(current_sound);
    current_sound->userData_float = volume;
    current_sound->userData_int = 1;

    return current_sound;
}

// -----------------------------------------

void pushsound_updateVolumeSettings(float _master_volume, float _music_volume) {
    master_volume = _master_volume;
    effect_volume = 1;
    music_volume = _music_volume;
    
    for (size_t i = 0; i < MAX_SOUNDS_COUNT; i++) {
        tsgl_sound* current_sound = &sounds[i];

        float newVolume = 0;
        if (current_sound->userData_int) {
            newVolume = calc_music_volume(current_sound->userData_float);
        } else {
            newVolume = calc_effect_volume(current_sound->userData_float);
        }

        tsgl_sound_setVolume(current_sound, newVolume);
    }

    for (size_t i = 0; i < MAX_NBS_COUNT; i++) {
        tsgl_nbs* current_sound = nbs_sounds[i];

        if (current_sound && current_sound->playing) {
            float newVolume = 0;
            if (current_sound->userData_int) {
                newVolume = calc_music_volume(current_sound->userData_float);
            } else {
                newVolume = calc_effect_volume(current_sound->userData_float);
            }

            tsgl_nbs_setVolume(current_sound, newVolume);
        }
    }
}
