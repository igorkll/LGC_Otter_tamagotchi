#pragma once
#include "hctl.h"

tsgl_sound* pushsound_getFreeSlot();
void pushsound_incrementSlot();
tsgl_sound* pushsound_load(const char* path, int sample_rate);
tsgl_sound* pushsound_play(const char* path, int sample_rate);
