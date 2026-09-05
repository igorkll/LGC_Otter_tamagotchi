#pragma once
#include "main.h"

extern tsgl_keyboard keyboard;
extern tsgl_sound_output* sound_output;

void hctl_init();
void hctl_setBacklight(uint8_t value);
void hctl_setBacklightAndWait(uint8_t value);
bool hctl_isBacklightChangeProcess();
void hctl_process();
void hctl_resetIdleTimer();
void hctl_enableAutoBacklight(bool autoBacklight);
