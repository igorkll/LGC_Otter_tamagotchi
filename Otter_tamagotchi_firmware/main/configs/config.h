#pragma once

#define TARGET_FPS 15
#define USE_HARDWARE_ROTATE false
#define ROTATE 0

#define BACKLIGHT_MAX 255
#define BACKLIGHT_IDLE 64
#define BACKLIGHT_OFF 0
#define BACKLIGHT_CHANGE_STEP 2
#define BACKLIGHT_CHANGE_PERIOD 10

#define BUTTON_0_PIN 9 //pressing when power is applied will switch to bootloader mode, this is a smart solution
#define BUTTON_1_PIN 10
//#define BUTTON_2_PIN 20
//#define BUTTON_3_PIN 21
#define BUTTON_2_PIN 2
#define BUTTON_3_PIN 3

#define SPEAKER_PIN 1

#define IDLE_AFTER_TIME 60000
#define KEYS_DEBOUNCE_PRESSING_MS 10
#define KEYS_DEBOUNCE_RELEASING_MS 30

#define SOUND_GLOBAL_TIMER_FREQ 16000
#define SOUND_BUFFER_SIZE (4 * 1024)
#define MAX_SOUNDS_COUNT 16
#define VOLUME_MUL 1

#define STARTUP_IMAGE_CHANGE_DELAY 3000

#define MYAAAA_SOUND_VOLUME 4
#define STARTUP_SOUND_VOLUME 1

//#define DEBUG_FPS
//#define DEBUG_TITLE
