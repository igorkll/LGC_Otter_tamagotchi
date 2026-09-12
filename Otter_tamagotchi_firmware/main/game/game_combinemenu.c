#include "game_combinemenu.h"
#include "game.h"
#include "../pushsound.h"
#include "../funcs.h"
#include "game_backpack.h"
#include "game_states.h"

#define COMBINEMENU_MARGIN_LEFT_RIGHT 20
#define COMBINEMENU_MARGIN_TOP_BOTTOM 40
#define COMBINEMENU_WIDTH (WIDTH - (COMBINEMENU_MARGIN_LEFT_RIGHT * 2))
#define COMBINEMENU_HEIGHT (HEIGHT - (COMBINEMENU_MARGIN_TOP_BOTTOM * 2))
#define COMBINEMENU_BORDER_SIZE 2

#define COMBINEMENU_FONT_TARGET_WIDTH 8
#define COMBINEMENU_FONT_TARGET_HEIGHT 8

static const char* combinemenu_points[] = {
    "\xD0\xFE\xEA\xE7\xE0\xEA", //Рюкзак
    "\xD1\xEE\xF1\xF2\xEE\xFF\xED\xE8\xE5", //Состояние
    "\xCD\xE0\xF1\xF2\xF0\xEE\xE9\xEA\xE8", //Настройки
};

#define POINTS_COUNT TSGL_CALC_ARRSIZE(combinemenu_points)
#define POINT_HEIGHT (COMBINEMENU_HEIGHT / POINTS_COUNT)

static tsgl_print_settings printsettings = {
    .locationMode = tsgl_print_start_top,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_bottom,
    .targetWidth = COMBINEMENU_FONT_TARGET_WIDTH,
    .targetHeight = COMBINEMENU_FONT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,
    .fg = TSGL_INVALID_RAWCOLOR,

    // multiline
    .multiline = true,
    .globalCentering = true,
    .width = COMBINEMENU_WIDTH,
    .height = POINT_HEIGHT
};

static void selectPoint(int index) {
    current_state.combinemenu_opened = false;
    game_updateActiveIcons();

    switch (index) {
        case 0:
            game_backpack_open();
            break;

        case 1:
            game_states_open();
            break;
    }
}

void game_combinemenu_draw()
{
    if (!current_state.combinemenu_opened)
        return;

    tsgl_pos x = (WIDTH / 2) - (COMBINEMENU_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (COMBINEMENU_HEIGHT / 2);

    tsgl_framebuffer_fill(&framebuffer, x, y, COMBINEMENU_WIDTH, COMBINEMENU_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, COMBINEMENU_WIDTH, COMBINEMENU_HEIGHT, white, COMBINEMENU_BORDER_SIZE);

    for (size_t index = 0; index < POINTS_COUNT; index++)
    {
        bool selected = index == current_state.combinemenu_index;
        printsettings.fg = selected ? black : green;

        tsgl_pos point_y = index * POINT_HEIGHT;
        tsgl_print_textArea textArea = tsgl_font_getTextArea(x, y + point_y, printsettings, combinemenu_points[index]);

        if (selected) {
            tsgl_framebuffer_fill(&framebuffer,
                x + (COMBINEMENU_BORDER_SIZE * 2),
                textArea.top - 2,
                COMBINEMENU_WIDTH - (COMBINEMENU_BORDER_SIZE * 4),
                textArea.height + 4,
                green
            );
        }
        tsgl_framebuffer_text(&framebuffer, x, y + point_y, printsettings, combinemenu_points[index]);
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_LEFT)) {
        current_state.combinemenu_index--;
        if (current_state.combinemenu_index < 0) current_state.combinemenu_index = 0;
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_OKAY)) {
        selectPoint(current_state.combinemenu_index);
    }

    if (tsgl_keyboard_whenPressed(&keyboard, KEY_INDEX_RIGHT)) {
        current_state.combinemenu_index++;
        if (current_state.combinemenu_index >= POINTS_COUNT) current_state.combinemenu_index = POINTS_COUNT - 1;
    }
}

void game_combinemenu_open()
{
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.combinemenu_opened = true;
    game_updateActiveIcons();
}

void game_combinemenu_close()
{
    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, EFFECTS_SOUND_VOLUME);
    current_state.combinemenu_opened = false;
    game_updateActiveIcons();
}

void game_combinemenu_toggle()
{
    if (current_state.combinemenu_opened)
    {
        game_combinemenu_close();
    }
    else
    {
        game_combinemenu_open();
    }
}
