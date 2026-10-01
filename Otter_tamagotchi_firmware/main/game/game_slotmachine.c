#include "game_slotmachine.h"
#include "../gfx.h"

void game_slotmachine_draw() {
    if (!current_state.overlay_slotmachine) return;
    
    gfx_drawCenteredImage(WIDTH / 2, HEIGHT / 2, "/firmware/images/slotmchn.bmp");
}

void game_slotmachine_open() {
    if (current_state.overlay_slotmachine) return;
    current_state.overlay_slotmachine = true;
    
}

void game_slotmachine_close() {
    if (!current_state.overlay_slotmachine) return;
    current_state.overlay_slotmachine = false;

}
