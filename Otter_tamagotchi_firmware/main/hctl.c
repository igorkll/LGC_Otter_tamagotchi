#include "hctl.h"

static uint8_t currentBackgroundValue = 0;
static uint8_t targetBackgroundValue = 0;
static TimerHandle_t change_backlight_timer_handle = NULL;

void hctl_init() {

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
            pdMS_TO_TICKS(100),
            pdTRUE,
            NULL, 
            change_backlight_timer_callback
        );
        if (change_backlight_timer_handle != NULL) xTimerStart(change_backlight_timer_handle, 0);
    }
}