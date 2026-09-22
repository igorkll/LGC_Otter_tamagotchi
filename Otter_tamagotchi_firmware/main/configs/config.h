#pragma once

#define TARGET_FPS 10

#define USE_HARDWARE_ROTATE false
#define ROTATE 0

#define PUSH_FUNC tsgl_framebuffer_pushFast_wtrans
#define PUSH_FUNC_TRANS tsgl_framebuffer_pushFast

//#define PUSH_FUNC tsgl_framebuffer_push_wtrans
//#define PUSH_FUNC_TRANS tsgl_framebuffer_push

#define CHECKPUSH_FUNC tsgl_framebuffer_push_wtrans
#define CHECKPUSH_FUNC_TRANS tsgl_framebuffer_push

// --------------------------- backlight

#define BACKLIGHT_MAX 255
#define BACKLIGHT_IDLE 64
#define BACKLIGHT_OFF 0
#define BACKLIGHT_CHANGE_STEP 2
#define BACKLIGHT_CHANGE_PERIOD 10

// --------------------------- gpio

#define BUTTON_0_PIN 9 //pressing when power is applied will switch to bootloader mode, this is a smart solution. this pin must be tightened with an external 10 kilohm resistor to 3.3 volts.
#define BUTTON_1_PIN 10
//#define BUTTON_2_PIN 20
//#define BUTTON_3_PIN 21
#define BUTTON_2_PIN 2
#define BUTTON_3_PIN 3

#define SPEAKER_PIN 1

// --------------------------- keys

#define KEYS_DEBOUNCE_PRESSING_MS 0
#define KEYS_DEBOUNCE_RELEASING_MS 30

// --------------------------- other

#define STARTUP_IMAGE_CHANGE_DELAY 3000
#define IDLE_AFTER_TIME 60000

// --------------------------- sound

#define SOUND_GLOBAL_TIMER_FREQ 64000
#define SOUND_BUFFER_SIZE (4 * 1024)
#define MAX_SOUNDS_COUNT 16
#define USE_SOUND_DOUBLE_BUFFER true
#define USE_SOUND_GLOBAL_TIMER true

// --------------------------- volume

#define VOLUME_MUL 0.5
#define EFFECTS_SOUND_VOLUME 1
#define MUSIC_SOUND_VOLUME 1

#define STARTUP_SOUND_VOLUME 0.5
#define MYAAAA_SOUND_VOLUME 1

#define TRIGGER_SOUND_VOLUME 0.6
#define QUESTION_SOUND_VOLUME 2.5
#define SADNESS_SOUND_VOLUME 2.2

#define BUY_SOUND_VOLUME 1

// --------------------------- debug

#define DEBUG_FPS
//#define DEBUG_TITLE
//#define DEBUG_PARAMS
//#define DEBUG_DISABLE_STARTUP_SOUND
