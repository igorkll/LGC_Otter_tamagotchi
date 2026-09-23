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
	nbs_pcm_callback callback;
	TaskHandle_t task;
	gptimer_handle_t timer;
	SemaphoreHandle_t mutex;
	Note notes[MAX_NOTES];
	float volume;
	void* userArg;
} NBS;

LoadedSamples* tsgl_nbs_loadSamples();
void tsgl_nbs_freeSamples(LoadedSamples* loadedSamples);

NBS* nbs_load(const char* path, nbs_pcm_callback callback, void* userArg);
void nbs_play(NBS* nbs);
void nbs_stop(NBS* nbs);
void nbs_free(NBS* nbs);
