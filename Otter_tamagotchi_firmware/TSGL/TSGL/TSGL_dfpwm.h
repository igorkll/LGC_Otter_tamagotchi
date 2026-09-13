#pragma once
#include "TSGL.h"

typedef struct {
    int8_t  charge;
    int8_t  strength;
    uint8_t last_bit;
} tsgl_dfpwm_decode_state;

void tsgl_dfpwm_reset(tsgl_dfpwm_decode_state* state);
int8_t tsgl_dfpwm_decode(tsgl_dfpwm_decode_state* state, uint8_t* buf, size_t bit_pos);
