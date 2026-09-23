#pragma once
#include "TSGL.h"
#include "TSGL_sound.h"

#define TSGL_NBS_MAX_ACTIVE_NOTES 32
#define TSGL_NBS_STACK_SIZE (1024 * 8)

typedef struct {
    tsgl_sound* samples;
    size_t count;
} tsgl_nbs_loadedSamples;

typedef struct {
    FILE* file;
    bool file_opened;

    TaskHandle_t task;
    bool task_created;

    char* path;
    
    tsgl_nbs_loadedSamples* loadedSamples;
    tsgl_sound active_notes[TSGL_NBS_MAX_ACTIVE_NOTES];

    float volume;
    tsgl_sound_output** outputs;
    size_t outputsCount;

    bool playing;
    bool loop;

    void* userData;
    int userData_int;
    float userData_float;
} tsgl_nbs;

tsgl_nbs_loadedSamples* tsgl_nbs_loadSamples(size_t count, const char* prefix, const char* suffix, size_t sample_rate, size_t bit_rate, size_t channels, tsgl_sound_pcm_format pcm_format);
void tsgl_nbs_freeSamples(tsgl_nbs_loadedSamples* loadedSamples);

tsgl_nbs* tsgl_nbs_load(tsgl_nbs_loadedSamples* loadedSamples, const char* path);
void tsgl_nbs_play(tsgl_nbs* nbs);
void tsgl_nbs_setOutputs(tsgl_nbs* nbs, tsgl_sound_output** outputs, size_t outputsCount);
void tsgl_nbs_setVolume(tsgl_nbs* nbs, float volume);
void tsgl_nbs_setLoop(tsgl_nbs* nbs, float loop);
void tsgl_nbs_free(tsgl_nbs* nbs);
