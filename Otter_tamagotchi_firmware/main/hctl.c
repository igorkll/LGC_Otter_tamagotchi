#include "hctl.h"

static int16_t currentBackgroundValue = 0;
static uint8_t targetBackgroundValue = 0;
static TimerHandle_t change_backlight_timer_handle = NULL;

static time_t lastInteractTime = 0;
static bool isIdle = false;
static bool oldIsIdle = false;

tsgl_keyboard keyboard;
tsgl_sound_output* sound_output;

static void change_backlight_timer_callback(TimerHandle_t xTimer) {
    if (targetBackgroundValue > currentBackgroundValue) {
        currentBackgroundValue += BACKLIGHT_CHANGE_STEP;
        if (currentBackgroundValue > targetBackgroundValue) currentBackgroundValue = targetBackgroundValue;
    } else if (targetBackgroundValue < currentBackgroundValue) {
        currentBackgroundValue -= BACKLIGHT_CHANGE_STEP;
        if (currentBackgroundValue < targetBackgroundValue) currentBackgroundValue = targetBackgroundValue;
    }
    tsgl_display_setBacklight(&display, currentBackgroundValue);

    if (currentBackgroundValue == targetBackgroundValue) {
        xTimerStop(xTimer, 0);
    }
}

void hctl_init() {
    time_t currentTime = tsgl_time();
    lastInteractTime = currentTime;

    // keyboard
    tsgl_keyboard_init(&keyboard);
    tsgl_keyboard_bindButton(&keyboard, 0, true, false, BUTTON_0_PIN);
    tsgl_keyboard_bindButton(&keyboard, 1, true, false, BUTTON_1_PIN);
    tsgl_keyboard_bindButton(&keyboard, 2, true, false, BUTTON_2_PIN);
    tsgl_keyboard_bindButton(&keyboard, 3, true, false, BUTTON_3_PIN);

    for (size_t i = 0; i < KEYS_COUNT; i++) {
        tsgl_keyboard_setDebounce(&keyboard, i, KEYS_DEBOUNCE_PRESSING_MS, KEYS_DEBOUNCE_RELEASING_MS);
    }

    // sound
    tsgl_sound_enableGlobalTimer(SOUND_GLOBAL_TIMER_FREQ, MAX_SOUNDS_COUNT);
    sound_output = tsgl_sound_newLedcOutput(SPEAKER_PIN);

    // backlight
    change_backlight_timer_handle = xTimerCreate(
        NULL,
        pdMS_TO_TICKS(BACKLIGHT_CHANGE_PERIOD),
        pdTRUE,
        NULL, 
        change_backlight_timer_callback
    );
}

void hctl_setBacklight(uint8_t value) {
    targetBackgroundValue = value;
    xTimerStart(change_backlight_timer_handle, 0);
}

bool hctl_isBacklightChangeProcess() {
    return currentBackgroundValue != targetBackgroundValue;
}

tsgl_sound* hctl_sound_play(const char* path, int sample_rate, float volume) {
    static tsgl_sound sounds[MAX_SOUNDS_COUNT] = {0};
    static uint8_t current_sound_index = 0;

    tsgl_sound* current_sound = &sounds[current_sound_index];
    if (current_sound->buffer) {
        tsgl_sound_free(current_sound);
    }

    if (tsgl_sound_load_pcmEx(current_sound, SOUND_BUFFER_SIZE, 0, path, sample_rate, 1, 1, tsgl_sound_pcm_unsigned, true) != ESP_OK) {
        return NULL;
    }
    
    current_sound_index++;
    if (current_sound_index >= MAX_SOUNDS_COUNT) {
        current_sound_index = 0;
    }

    tsgl_sound_output* sound_outputs[] = {sound_output};
    tsgl_sound_enableFreeOnEnd(current_sound, true);
    tsgl_sound_setOutputs(current_sound, sound_outputs, 1, false);
    tsgl_sound_setVolume(current_sound, volume);
    tsgl_sound_play(current_sound);

    return current_sound;
}

void hctl_process() {
    tsgl_keyboard_readAll(&keyboard);

    time_t currentTime = tsgl_time();
    for (size_t i = 0; i < KEYS_COUNT; i++) {
        if (tsgl_keyboard_whenPressed(&keyboard, i)) {
            lastInteractTime = currentTime;
        }
    }

    isIdle = tsgl_time() - lastInteractTime > IDLE_AFTER_TIME;
    if (isIdle != oldIsIdle) {
        hctl_setBacklight(isIdle ? BACKLIGHT_IDLE : BACKLIGHT_MAX);
        oldIsIdle = isIdle;
    }
}
