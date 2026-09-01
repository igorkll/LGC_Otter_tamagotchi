#include "game_upmenu.h"
#include "game.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../funcs.h"

static tsgl_sprite* sprites[GAME_UPMENU_COUNTS_COUNT];
static bool sprites_active[GAME_UPMENU_COUNTS_COUNT];
static tsgl_sprite* sprite_iconline;
static int current_selected = -1;

#define ICON_FAILBACK "/storage/icons/null.bmp"
#define FRAME2_LINE_LEN 5
#define FRAME2_LINE_OFFSET (FRAME2_LINE_LEN - 1)

void game_upmenu_reloadIcons() {
    for (size_t i = 0; i < GAME_UPMENU_COUNTS_COUNT; i++) {
        char path[MAX_PATH_LEN];
        slnprintf(path, MAX_PATH_LEN, "/storage/icons/%s/%i.bmp", game_getCurrentRoom()->background, i);
        if (!tsgl_filesystem_exists(path)) slnprintf(path, MAX_PATH_LEN, "/storage/icons/%i.bmp", i);
        if (!tsgl_filesystem_exists(path)) slnprintf(path, MAX_PATH_LEN, ICON_FAILBACK, i);

        if (sprites[i] != NULL)
            tsgl_bmp_free(sprites[i]);

        tsgl_sprite* sprite = gfx_loadSprite(path);
        if (sprite == NULL) sprite = gfx_loadSprite(ICON_FAILBACK);
        sprites[i] = sprite;
    }
}

void game_upmenu_init() {
    sprite_iconline = gfx_loadSprite("/storage/images/iconline.bmp");
    game_upmenu_reloadIcons();
}

int game_upmenu_process() {
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

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_CANCEL)) {
        current_selected = -1;
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_OKAY)) {
        return current_selected;
    }

    return -1;
}

static void draw_icons(int offsetIndex, int offsetHeight, int selected) {
    int lineHeight = sprite_iconline->sprite->height;

    for (size_t i = 0; i < GAME_UPMENU_LINE_COUNTS_COUNT; i++) {
        size_t i2 = i + offsetIndex;

        int iconWidth = sprites[0]->sprite->width;
        int iconHeight = sprites[0]->sprite->height;

        int x = ((width / 2) - (iconWidth / 2)) + ((i - (GAME_UPMENU_LINE_COUNTS_COUNT / 2)) * (width / 5));
        int y = offsetHeight + ((lineHeight / 2) - (iconHeight / 2));

        tsgl_rawcolor fillColor = sprites_active[i2] ? red : transparent;
        tsgl_rawcolor fillColor2 = i2 == selected ? yellow : fillColor;

        tsgl_pos posX = x - 1;
        tsgl_pos posY = y - 1;
        tsgl_pos fillSizeX = iconWidth + 2;
        tsgl_pos fillSizeY = iconHeight + 2;
        tsgl_pos pos2X = posX + (fillSizeX - 1);
        tsgl_pos pos2Y = posY + (fillSizeY - 1);

        // main border
        tsgl_framebuffer_fill(&framebuffer, posX, posY, fillSizeX, fillSizeY, fillColor);

        // corner border
        // left & top
        tsgl_framebuffer_fill(&framebuffer, posX, posY,                             FRAME2_LINE_LEN, 1, fillColor2);
        tsgl_framebuffer_fill(&framebuffer, posX, posY,                             1, FRAME2_LINE_LEN, fillColor2);

        // right & bottom
        tsgl_framebuffer_fill(&framebuffer, pos2X - FRAME2_LINE_OFFSET, pos2Y,      FRAME2_LINE_LEN, 1, fillColor2);
        tsgl_framebuffer_fill(&framebuffer, pos2X, pos2Y - FRAME2_LINE_OFFSET,      1, FRAME2_LINE_LEN, fillColor2);

        // left & bottom
        tsgl_framebuffer_fill(&framebuffer, posX, pos2Y,                            FRAME2_LINE_LEN, 1, fillColor2);
        tsgl_framebuffer_fill(&framebuffer, posX, pos2Y - FRAME2_LINE_OFFSET,       1, FRAME2_LINE_LEN, fillColor2);

        // right & top
        tsgl_framebuffer_fill(&framebuffer, pos2X, posY,                            1, FRAME2_LINE_LEN, fillColor2);
        tsgl_framebuffer_fill(&framebuffer, pos2X - FRAME2_LINE_OFFSET, posY,       FRAME2_LINE_LEN, 1, fillColor2);

        // icon
        tsgl_framebuffer_pushFast(&framebuffer, x, y, sprites[i2]);
    }
}

void game_upmenu_draw() {
    int lineHeight = sprite_iconline->sprite->height;
    int bottomLineY = height - lineHeight;

    tsgl_framebuffer_pushFast(&framebuffer, 0, 0, sprite_iconline);
    tsgl_framebuffer_pushFast(&framebuffer, 0, bottomLineY, sprite_iconline);
    
    draw_icons(0, 0, current_selected);
    draw_icons(GAME_UPMENU_LINE_COUNTS_COUNT, bottomLineY, current_selected);
}

int game_upmenu_currentSelected() {
    return current_selected;
}

void game_upmenu_setActivate(int index, bool state) {
    sprites_active[index] = state;
}

bool game_upmenu_isActivate(int index) {
    return sprites_active[index];
}
