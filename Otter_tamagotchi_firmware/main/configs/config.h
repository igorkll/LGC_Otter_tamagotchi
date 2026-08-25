#pragma once

#define USE_HARDWARE_ROTATE false
#define ROTATE 0

#define BACKLIGHT_MAX 255
#define BACKLIGHT_IDLE 64
#define BACKLIGHT_CHANGE_STEP 2
#define BACKLIGHT_CHANGE_PERIOD 10

#define BUTTON_0_PIN 9 //pressing when power is applied will switch to bootloader mode, this is a smart solution
#define BUTTON_1_PIN 10
#define BUTTON_2_PIN 20
#define BUTTON_3_PIN 21

#define SPEAKER_PIN 1

#define IDLE_AFTER_TIME 10000
#define KEYS_DEBOUNCE_PRESSING_MS 50
#define KEYS_DEBOUNCE_RELEASING_MS 50

#define SOUND_GLOBAL_TIMER_FREQ 16000
#define MAX_SOUNDS_COUNT 16
