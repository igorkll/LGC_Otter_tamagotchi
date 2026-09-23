#pragma once
#include "hctl.h"

void pushsound_initNbs();

tsgl_sound* pushsound_getFreeSlot();
void pushsound_incrementSlot();
tsgl_sound* pushsound_load(const char* path, int sample_rate);
tsgl_sound* pushsound_play(const char* path, int sample_rate, float volume);
tsgl_sound* pushsound_loop(const char* path, int sample_rate, float volume);

size_t pushsound_nbs_getFreeSlot();
void pushsound_nbs_incrementSlot();
tsgl_nbs* pushsound_nbs_load(const char* path);
tsgl_nbs* pushsound_nbs_play(const char* path, float volume);
tsgl_nbs* pushsound_nbs_loop(const char* path, float volume);

void pushsound_updateVolumeSettings(float master_volume, float music_volume);
