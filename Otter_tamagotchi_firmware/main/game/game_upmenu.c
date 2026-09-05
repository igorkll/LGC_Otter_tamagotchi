#include "game_option_description.h"
#include "game_upmenu.h"
#include "game.h"
#include "../gfx.h"
#include "../hctl.h"
#include "../funcs.h"

static tsgl_sprite* sprites[GAME_UPMENU_COUNT];
static bool sprites_active[GAME_UPMENU_COUNT];
static tsgl_sprite* sprite_iconline;
static int current_selected = -1;

#define FRAME2_LINE_LEN 5
#define FRAME2_LINE_OFFSET (FRAME2_LINE_LEN - 1)
#define OPTION_DESCRIPTION_MARGIN 5
#define OPTION_DESCRIPTION_POS 26
#define OPTION_DESCRIPTION_POS_DOWN (HEIGHT - OPTION_DESCRIPTION_POS)
#define OPTION_DESCRIPTION_WIDTH (WIDTH * 0.8)
#define OPTION_DESCRIPTION_HEIGHT 25
#define OPTION_DESCRIPTION_TEXT_TARGET_WIDTH 10
#define OPTION_DESCRIPTION_TEXT_TARGET_HEIGHT 16

void game_upmenu_reloadIcons() {
    for (size_t i = 0; i < GAME_UPMENU_COUNT; i++) {
        char path[MAX_PATH_LEN];
        path[0] = '\0';

        slnprintf(path, MAX_PATH_LEN, "/firmware/icons/%s/%i.bmp", game_getCurrentRoom()->background, i);
        if (!game_isLockedInRoom() && !tsgl_filesystem_exists(path))
            slnprintf(path, MAX_PATH_LEN, "/firmware/icons/%i.bmp", i);

        if (sprites[i] != NULL)
            tsgl_bmp_free(sprites[i]);

        if (path[0] != '\0' && tsgl_filesystem_exists(path)) {
            sprites[i] = gfx_loadSprite(path);
        } else {
            sprites[i] = NULL;
        }
    }
}

void game_upmenu_init() {
    sprite_iconline = gfx_loadSprite("/firmware/images/iconline.bmp");
    game_upmenu_reloadIcons();
}

int game_upmenu_process() {
    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_LEFT)) {
        current_selected--;
        if (current_selected < 0) {
            current_selected = GAME_UPMENU_COUNT - 1;
        }
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_RIGHT)) {
        current_selected++;
        if (current_selected >= GAME_UPMENU_COUNT) {
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

static tsgl_sprite* renderedOptionDescription;
static int old_selectingIndex = -1;

static tsgl_print_settings printsettings_upselect = {
    .multiline = true,
    .locationMode = tsgl_print_start_top,
    .width = OPTION_DESCRIPTION_WIDTH,
    .height = OPTION_DESCRIPTION_HEIGHT,
    .globalAlignmentX = tsgl_print_alignment_center,
    .globalAlignmentY = tsgl_print_alignment_center,
    .alignment = tsgl_print_alignment_center,
    
    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = OPTION_DESCRIPTION_TEXT_TARGET_WIDTH,
    .targetHeight = OPTION_DESCRIPTION_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

static void draw_option_description(int selectingIndex) {
    tsgl_pos positionX = (WIDTH / 2) - (OPTION_DESCRIPTION_WIDTH / 2);
    tsgl_pos position = selectingIndex < GAME_UPMENU_LINE_COUNT ? (OPTION_DESCRIPTION_POS + OPTION_DESCRIPTION_MARGIN) : (OPTION_DESCRIPTION_POS_DOWN - OPTION_DESCRIPTION_HEIGHT - OPTION_DESCRIPTION_MARGIN);

    size_t currentRoom = game_getCurrentRoomIndex();

    const char* text = NULL;
    if (currentRoom < GAME_UPMENU_LINE_COUNT && selectingIndex < GAME_UPMENU_LINE_COUNT) {
        text = game_rooms_option_descriptions_main_rooms[selectingIndex];
    } else {
        text = game_rooms_option_descriptions[currentRoom].arr[selectingIndex];
    }

    if (renderedOptionDescription && selectingIndex != old_selectingIndex) {
        old_selectingIndex = selectingIndex;
        tsgl_bmp_free(renderedOptionDescription);
        renderedOptionDescription = NULL;
    }

    if (text != NULL) {
        printsettings_upselect.fg = white;
        if (renderedOptionDescription == NULL) renderedOptionDescription = tsgl_gfx_renderTextToSprite(0, 0, OPTION_DESCRIPTION_WIDTH, OPTION_DESCRIPTION_HEIGHT, printsettings_upselect, text, framebuffer.colormode, 0, TSGL_INVALID_RAWCOLOR, black);
        PUSH_FUNC(&framebuffer, positionX, position, renderedOptionDescription);
    }
}

static void draw_icons(int offsetIndex, int offsetHeight, int selected) {
    int lineHeight = sprite_iconline->sprite->height;

    int iconWidth = 20;
    int iconHeight = 20;

    time_t uptime = tsgl_time();
    for (size_t i = 0; i < GAME_UPMENU_LINE_COUNT; i++) {
        size_t i2 = i + offsetIndex;

        int x = ((width / 2) - (iconWidth / 2)) + ((i - (GAME_UPMENU_LINE_COUNT / 2)) * (width / 5));
        int y = offsetHeight + ((lineHeight / 2) - (iconHeight / 2));

        tsgl_rawcolor borderColor = sprites_active[i2] ? red : transparent;
        tsgl_rawcolor cornersColor = uptime % 1000 >= 500 ? magenta : yellow;
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
        if (sprite) PUSH_FUNC(&framebuffer, x, y, sprite);
    }
}

void game_upmenu_draw() {
    int lineHeight = sprite_iconline->sprite->height;
    int bottomLineY = height - lineHeight;

    PUSH_FUNC(&framebuffer, 0, 0, sprite_iconline);
    PUSH_FUNC(&framebuffer, 0, bottomLineY, sprite_iconline);
    
    
    draw_icons(0, 0, current_selected);
    draw_icons(GAME_UPMENU_LINE_COUNT, bottomLineY, current_selected);
    if (current_selected >= 0) draw_option_description(current_selected);
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

void game_upmenu_redrawTitle() {
    old_selectingIndex = -1;
}
