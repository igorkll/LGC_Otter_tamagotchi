#pragma once
#include "../main.h"

void game_states_draw();
void game_states_open();
void game_states_close();
void game_states_toggle();

void game_states_change(game_state_val* ptr, game_state_val delta);
