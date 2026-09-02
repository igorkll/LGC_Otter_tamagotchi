#include "game_upmenu.h"
#include "game.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../funcs.h"

static tsgl_sprite* sprites[GAME_UPMENU_COUNTS_COUNT];
static bool sprites_active[GAME_UPMENU_COUNTS_COUNT];
static tsgl_sprite* sprite_iconline;
static int current_selected = -1;

#define ICON_FAILBACK "/storage/icons/failback.bmp"
#define FRAME2_LINE_LEN 5
#define FRAME2_LINE_OFFSET (FRAME2_LINE_LEN - 1)

void game_upmenu_reloadIcons() {
    for (size_t i = 0; i < GAME_UPMENU_COUNTS_COUNT; i++) {
        char path[MAX_PATH_LEN];
        path[0] = '\0';

        if (!game_isLockedInRoom() || i >= ROOMS_COUNT_AVAILABLE_FOR_MANUAL_SELECT) {
            slnprintf(path, MAX_PATH_LEN, "/storage/icons/%s/%i.bmp", game_getCurrentRoom()->background, i);
            if (!tsgl_filesystem_exists(path)) slnprintf(path, MAX_PATH_LEN, "/storage/icons/%i.bmp", i);
        }

        if (sprites[i] != NULL)
            tsgl_bmp_free(sprites[i]);

        if (tsgl_filesystem_exists(path)) {
            tsgl_sprite* sprite = gfx_loadSprite(path);
            if (sprite == NULL) sprite = gfx_loadSprite(ICON_FAILBACK);
            sprites[i] = sprite;
        } else {
            sprites[i] = NULL;
        }
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

    int iconWidth = 20;
    int iconHeight = 20;

    time_t uptime = tsgl_time();
    for (size_t i = 0; i < GAME_UPMENU_LINE_COUNTS_COUNT; i++) {
        size_t i2 = i + offsetIndex;

        int x = ((width / 2) - (iconWidth / 2)) + ((i - (GAME_UPMENU_LINE_COUNTS_COUNT / 2)) * (width / 5));
        int y = offsetHeight + ((lineHeight / 2) - (iconHeight / 2));

        tsgl_rawcolor borderColor = sprites_active[i2] ? red : transparent;
        tsgl_rawcolor cornersColor = uptime % 1000 >= 500 ? blue : yellow;
        tsgl_rawcolor currentCordersColor = i2 == selected ? cornersColor : borderColor;

        tsgl_pos posX = x - 1;
        tsgl_pos posY = y - 1;
        tsgl_pos fillSizeX = iconWidth + 2;
        tsgl_pos fillSizeY = iconHeight + 2;
        tsgl_pos pos2X = posX + (fillSizeX - 1);
        tsgl_pos pos2Y = posY + (fillSizeY - 1);

        // main border
        tsgl_framebuffer_fill(&framebuffer, posX, posY, fillSizeX, fillSizeY, borderColor);

        // corner border
        // left & top
        tsgl_framebuffer_fill(&framebuffer, posX, posY,                             FRAME2_LINE_LEN, 1, currentCordersColor);
        tsgl_framebuffer_fill(&framebuffer, posX, posY,                             1, FRAME2_LINE_LEN, currentCordersColor);

        // right & bottom
        tsgl_framebuffer_fill(&framebuffer, pos2X - FRAME2_LINE_OFFSET, pos2Y,      FRAME2_LINE_LEN, 1, currentCordersColor);
        tsgl_framebuffer_fill(&framebuffer, pos2X, pos2Y - FRAME2_LINE_OFFSET,      1, FRAME2_LINE_LEN, currentCordersColor);

        // left & bottom
        tsgl_framebuffer_fill(&framebuffer, posX, pos2Y,                            FRAME2_LINE_LEN, 1, currentCordersColor);
        tsgl_framebuffer_fill(&framebuffer, posX, pos2Y - FRAME2_LINE_OFFSET,       1, FRAME2_LINE_LEN, currentCordersColor);

        // right & top
        tsgl_framebuffer_fill(&framebuffer, pos2X, posY,                            1, FRAME2_LINE_LEN, currentCordersColor);
        tsgl_framebuffer_fill(&framebuffer, pos2X - FRAME2_LINE_OFFSET, posY,       FRAME2_LINE_LEN, 1, currentCordersColor);

        // icon
        tsgl_sprite* sprite = sprites[i2];
        if (sprite) tsgl_framebuffer_pushFast(&framebuffer, x, y, sprite);
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
