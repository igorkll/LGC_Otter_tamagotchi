#include "game_combinemenu.h"
#include "game.h"
#include "../pushsound.h"
#include "../funcs.h"

#define COMBINEMENU_MARGIN_LEFT_RIGHT 20
#define COMBINEMENU_MARGIN_TOP_BOTTOM 40
#define COMBINEMENU_WIDTH (WIDTH - (COMBINEMENU_MARGIN_LEFT_RIGHT * 2))
#define COMBINEMENU_HEIGHT (HEIGHT - (COMBINEMENU_MARGIN_TOP_BOTTOM * 2))
#define COMBINEMENU_BORDER_SIZE 2

#define COMBINEMENU_FONT_TARGET_WIDTH 8
#define COMBINEMENU_FONT_TARGET_HEIGHT 8

static const char *combinemenu_points = {
    "1",
    "2",
    "3",
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

    // multiline
    .multiline = true,
    .globalCentering = true,
    .width = COMBINEMENU_WIDTH,
    .height = };

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
        tsgl_pos point_y = index * POINT_HEIGHT;
        tsgl_framebuffer_text(x, y + point_y, printsettings, combinemenu_points[index]);
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
