#include "hctl.h"

static int16_t currentBackgroundValue = 0;
static uint8_t targetBackgroundValue = 0;
static TimerHandle_t change_backlight_timer_handle = NULL;

tsgl_keyboard keyboard;
static time_t lastInteractTime = 0;
static bool isIdle = false;

void hctl_init() {
    time_t currentTime = tsgl_time();

    lastInteractTime = currentTime;

    tsgl_keyboard_init(&keyboard);
    tsgl_keyboard_bindButton(&keyboard, 0, true, false, BUTTON_0_PIN);
    tsgl_keyboard_bindButton(&keyboard, 1, true, false, BUTTON_1_PIN);
    tsgl_keyboard_bindButton(&keyboard, 2, true, false, BUTTON_2_PIN);
    tsgl_keyboard_bindButton(&keyboard, 3, true, false, BUTTON_3_PIN);

    for (size_t i = 0; i < KEYS_COUNT; i++) {
        tsgl_keyboard_setDebounce(&keyboard, i, KEYS_DEBOUNCE_PRESSING_MS, KEYS_DEBOUNCE_RELEASING_MS);
    }
}

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
        xTimerDelete(xTimer, 0);
        change_backlight_timer_handle = NULL;
    }
}

void hctl_setBacklight(uint8_t value) {
    targetBackgroundValue = value;
    
    if (change_backlight_timer_handle == NULL) {
        change_backlight_timer_handle = xTimerCreate(
            NULL,
            pdMS_TO_TICKS(BACKLIGHT_CHANGE_PERIOD),
            pdTRUE,
            NULL, 
            change_backlight_timer_callback
        );
        if (change_backlight_timer_handle != NULL) xTimerStart(change_backlight_timer_handle, 0);
    }
}

bool hctl_isBacklightChangeProcess() {
    return change_backlight_timer_handle != NULL;
}

void hctl_process() {
    tsgl_keyboard_readAll(&keyboard);

    time_t currentTime = tsgl_time();
    for (size_t i = 0; i < KEYS_COUNT; i++) {
        if (tsgl_keyboard_whenPressed(&keyboard, i)) {
            lastInteractTime = currentTime;
        }
    }

    bool isIdle = tsgl_time() - lastInteractTime > IDLE_AFTER_TIME;
    hctl_setBacklight(isIdle ? BACKLIGHT_IDLE : BACKLIGHT_MAX);
}
