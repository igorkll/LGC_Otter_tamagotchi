#pragma once
#include "main.h"

#define KEYS_COUNT 4
#define KEY_INDEX_LEFT 0
#define KEY_INDEX_OKAY 1
#define KEY_INDEX_CANCEL 2
#define KEY_INDEX_RIGHT 3

extern tsgl_keyboard keyboard;
extern tsgl_sound_output* sound_output;

void hctl_init();
void hctl_setBacklight(uint8_t value);
bool hctl_isBacklightChangeProcess();
void hctl_process();
