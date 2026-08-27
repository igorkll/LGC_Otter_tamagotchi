#include "game_upmenu.h"
#include "../gfx.h"

static tsgl_sprite* sprites[GAME_UPMENU_COUNTS_COUNT];
static bool sprites_active[GAME_UPMENU_COUNTS_COUNT];
static tsgl_sprite* sprite_iconline;
static int current_selected = 0;

void game_upmenu_init() {
    for (size_t i = 0; i < GAME_UPMENU_COUNTS_COUNT; i++) {
        char path[40] = "";
        sprintf(path, "/storage/icons/%i.bmp", i);
        tsgl_sprite* sprite = gfx_loadSprite(path);
        if (sprite == NULL) {
            sprite = gfx_loadSprite("/storage/icons/null.bmp");
        }
        sprites[i] = sprite;
    }
    sprite_iconline = gfx_loadSprite("/storage/images/iconline.bmp");
}

void game_upmenu_process() {
    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_LEFT)) {
        current_selected--;
        if (current_selected < 0) {
            current_selected = GAME_UPMENU_COUNTS_COUNT - 1;
        }
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_RIGHT)) {
        current_selected++;
        if (current_selected >= GAME_UPMENU_COUNTS_COUNT) {
            current_selected = 0;
        }
    }
}

static void draw_icons(int offsetIndex, int offsetHeight, int selected) {
    int lineHeight = sprite_iconline->sprite->height;

    for (size_t i = 0; i < GAME_UPMENU_LINE_COUNTS_COUNT; i++) {
        size_t i2 = i + offsetIndex;

        int iconWidth = sprites[0]->sprite->width;
        int iconHeight = sprites[0]->sprite->height;

        int x = ((width / 2) - (iconWidth / 2)) + ((i - (GAME_UPMENU_LINE_COUNTS_COUNT / 2)) * (width / 5));
        int y = offsetHeight + ((lineHeight / 2) - (iconHeight / 2));

        if (i2 == selected) {
            tsgl_rawcolor fillColor = sprites_active[i2] ? red : yellow;
            tsgl_framebuffer_fill(&framebuffer, x - 1, y - 1, iconWidth + 2, iconHeight + 2, fillColor);
        }
        tsgl_framebuffer_push(&framebuffer, x, y, sprites[i2]);
    }
}

void game_upmenu_draw() {
    int lineHeight = sprite_iconline->sprite->height;
    int bottomLineHeight = height - lineHeight;

    tsgl_framebuffer_push(&framebuffer, 0, 0, sprite_iconline);
    tsgl_framebuffer_push(&framebuffer, 0, bottomLineHeight, sprite_iconline);
    
    draw_icons(0, 0, current_selected);
    draw_icons(GAME_UPMENU_LINE_COUNTS_COUNT, bottomLineHeight, current_selected);
}

int game_upmenu_currentSelected() {
    return current_selected;
}

void game_upmenu_setActivate(int index, bool state) {
    sprites_active[index] = state
}

bool game_upmenu_isActivate(int index) {
    return sprites_active[index];
}
