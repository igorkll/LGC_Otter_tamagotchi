#pragma once
#include "TSGL.h"
#include "TSGL_sound.h"

#define TSGL_NBS_MAX_ACTIVE_NOTES 32
#define TSGL_NBS_STACK_SIZE 4096

typedef struct {
    tsgl_sound* samples;
    size_t count;
} LoadedSamples;

typedef struct {
    FILE* file;
    TaskHandle_t task;
    
    LoadedSamples* loadedSamples;
    tsgl_sound active_notes[TSGL_NBS_MAX_ACTIVE_NOTES];

    tsgl_sound_output** outputs;
    size_t outputsCount;

    void* userData;
    int userData_int;
    float userData_float;
} NBS;

LoadedSamples* tsgl_nbs_loadSamples(size_t count, const char* prefix, const char* suffix, size_t sample_rate, size_t bit_rate, size_t channels, tsgl_sound_pcm_format pcm_format);
void tsgl_nbs_freeSamples(LoadedSamples* loadedSamples);

NBS* tsgl_nbs_load(LoadedSamples* loadedSamples, const char* path);
void tsgl_nbs_start();
void tsgl_nbs_setOutputs(NBS* nbs, tsgl_sound_output** outputs, size_t outputsCount);
void tsgl_nbs_free(NBS* nbs);
