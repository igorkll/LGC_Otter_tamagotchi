#pragma once
#include "../main.h"
#include "game.h"

int shop_getCurrentItem();
int shop_getItemPrice(int itemNum);
int* shop_getItemPtr(int itemNum);
void shop_draw_overlay();
bool shop_buy(int* countvar, int price);
