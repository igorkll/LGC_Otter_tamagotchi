#pragma once
#include "main.h"

#define KEYS_COUNT 4

extern tsgl_keyboard keyboard;
extern tsgl_sound_output* sound_output;

void hctl_init();
void hctl_setBacklight(uint8_t value);
bool hctl_isBacklightChangeProcess();
void hctl_process();
tsgl_sound* hctl_sound_play(const char* path, int sample_rate, float volume);
