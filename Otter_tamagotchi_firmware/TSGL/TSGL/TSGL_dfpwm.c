#include "TSGL_dfpwm.h"

void tsgl_dfpwm_reset(tsgl_dfpwm_decode_state* state) {
    
}

int8_t tsgl_dfpwm_decode(tsgl_dfpwm_decode_state* state, uint8_t* buf, size_t bit_pos) {
    // Достаём бит: bit_pos — глобальный индекс, LSB-first внутри байта
    uint8_t bit = (buf[bit_pos >> 3] >> (bit_pos & 7)) & 1;

    int16_t charge   = state->charge;
    int16_t strength = state->strength;

    // Двигаем charge к биту (экспоненциальный подход к границам)
    if (bit) {
        charge += (strength * (127 - charge)) >> 7;
    } else {
        charge -= (strength * (charge + 128)) >> 7;
    }

    // Клиппинг charge
    if (charge > 127)  charge = 127;
    if (charge < -128) charge = -128;

    // Адаптация strength — только вне насыщения
    if (charge != 127 && charge != -128) {
        if (state->last_bit == bit) strength--;
        else                        strength++;
        if (strength < 0)   strength = 0;
        if (strength > 127) strength = 127;
    }

    state->charge   = charge;
    state->strength = strength;
    state->last_bit = bit;

    return charge;
}
