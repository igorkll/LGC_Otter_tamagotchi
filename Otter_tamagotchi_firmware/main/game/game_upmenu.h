#pragma once
#include "../main.h"

#define GAME_UPMENU_LINE_COUNTS_COUNT 5
#define GAME_UPMENU_COUNTS_COUNT (GAME_UPMENU_LINE_COUNTS_COUNT * 2)

void game_upmenu_init();
void game_upmenu_reloadIcons();
int game_upmenu_process();
void game_upmenu_draw();

int game_upmenu_currentSelected();
void game_upmenu_setActivate(int index, bool state);
bool game_upmenu_isActivate(int index);
