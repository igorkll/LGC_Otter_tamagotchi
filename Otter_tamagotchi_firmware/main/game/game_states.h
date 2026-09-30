#pragma once
#include "../main.h"
#include "game.h"

void game_states_draw();
void game_states_open();
void game_states_close();
void game_states_toggle();

void game_states_change(game_state_val* ptr, game_state_val delta);

bool game_states_is_fatigue_critical();
bool game_states_is_hunger_critical();
bool game_states_is_thirst_critical();
bool game_states_is_caress_critical();
bool game_states_is_sadness_critical();