#include "game_option_description.h"

const char* car_str = "\xCC\xE0\xF8\xE8\xED\xE0"; //Машина
const char* yard_str = "\xC4\xE2\xEE\xF0"; //Двор
const char* shop_str = "\xCC\xE0\xE3\xE0\xE7\xE8\xED"; //Магазин

const char* game_rooms_option_descriptions_main_rooms[GAME_UPMENU_LINE_COUNT] = {
    "\xD1\xEF\xE0\xEB\xFC\xED\xFF", //Спальня
    "\xCA\xF3\xF5\xED\xFF", //Кухня
    "\xCA\xEE\xEC\xED\xE0\xF2\xE0", //Комната
    "\xD2\xF3\xE0\xEB\xE5\xF2", //Туалет
    yard_str //Двор
};

Room_option_descriptions game_rooms_option_descriptions[] = {
    { //Спальня
        .arr = {
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    { //Кухня
        .arr = {
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    { //Комната
        .arr = {
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    { //Туалет
        .arr = {
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    { //Двор
        .arr = {
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            car_str,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    { //Машина
        .arr = {
            yard_str,
            shop_str,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    { //Магазин
        .arr = {
            car_str,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    }
};
