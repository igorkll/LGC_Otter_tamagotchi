#pragma once
#include "main.h"

#define KEYS_COUNT 4

extern tsgl_keyboard keyboard;

void hctl_init();
void hctl_setBacklight(uint8_t value);
bool hctl_isBacklightChangeProcess();
void hctl_process();
