#include "game_option_description.h"

#define CAR_STR   "\xCC\xE0\xF8\xE8\xED\xE0"
#define YARD_STR  "\xC4\xE2\xEE\xF0"
#define SHOP_STR  "\xCC\xE0\xE3\xE0\xE7\xE8\xED"

const char* game_rooms_option_descriptions_main_rooms[GAME_UPMENU_LINE_COUNT] = {
    "\xD1\xEF\xE0\xEB\xFC\xED\xFF", //Спальня
    "\xCA\xF3\xF5\xED\xFF", //Кухня
    "\xCA\xEE\xEC\xED\xE0\xF2\xE0", //Комната
    "\xD2\xF3\xE0\xEB\xE5\xF2", //Туалет
    YARD_STR //Двор
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
            CAR_STR,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    { //Машина
        .arr = {
            YARD_STR,
            SHOP_STR,
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
            CAR_STR,
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
