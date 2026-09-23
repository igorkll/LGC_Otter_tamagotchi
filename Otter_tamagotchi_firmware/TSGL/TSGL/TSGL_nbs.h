#pragma once
#include "TSGL.h"
#include "TSGL_sound.h"

#define MAX_NOTES 32

typedef struct {
    tsgl_sound* samples;
    size_t count;
} LoadedSamples;

typedef struct {
    uint8_t instrument;
    float index;
    float step;
    bool play;
} Note;

typedef struct {
    FILE* file;
    TaskHandle_t task;
    Note notes[MAX_NOTES];

    void* userData;
    int userData_int;
    float userData_float;
} NBS;

LoadedSamples* tsgl_nbs_loadSamples(size_t count, const char* prefix, const char* suffix, size_t sample_rate, size_t bit_rate, size_t channels, tsgl_sound_pcm_format pcm_format);
void tsgl_nbs_attachSamplesToOutputs(LoadedSamples* loadedSamples, tsgl_sound_output** outputs, size_t outputsCount, bool freeOutputs);
void tsgl_nbs_freeSamples(LoadedSamples* loadedSamples);
