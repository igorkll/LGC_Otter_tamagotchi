#include "game_achievements.h"
#include "game.h"
#include "gfx.h"
#include "../pushsound.h"

#define ACHIEVEMENTS_BLOCK_MARGIN 4
#define ACHIEVEMENTS_BLOCK_HEIGHT 40
#define ACHIEVEMENTS_BLOCKS_COUNT 3

#define ACHIEVEMENTS_WIDTH (WIDTH - 4)
#define ACHIEVEMENTS_BORDER_SIZE 2
#define ACHIEVEMENTS_HEIGHT (((ACHIEVEMENTS_BLOCK_HEIGHT + ACHIEVEMENTS_BLOCK_MARGIN) * ACHIEVEMENTS_BLOCKS_COUNT) + (ACHIEVEMENTS_BORDER_SIZE * 2))

#define ACHIEVEMENTS_ICON_TEXT_WIDTH 6
#define ACHIEVEMENTS_ICON_TEXT_HEIGHT 10
#define ACHIEVEMENTS_ICON_SMALL_TEXT_WIDTH 5
#define ACHIEVEMENTS_ICON_SMALL_TEXT_HEIGHT 10
#define ACHIEVEMENTS_ICON_BORDER_SIZE 1

#define ACHIEVEMENTS_BLOCK_WIDTH (ACHIEVEMENTS_WIDTH - (ACHIEVEMENTS_BLOCK_MARGIN * 2))
#define ACHIEVEMENTS_BLOCK_BORDER_SIZE 1
#define ACHIEVEMENTS_IMAGE_BORDER_SIZE 1

#define ACHIEVEMENTS_BLOCK_IMAGE_MARGIN 4
#define ACHIEVEMENTS_BLOCK_IMAGE_SIZE (ACHIEVEMENTS_BLOCK_HEIGHT - (ACHIEVEMENTS_BLOCK_IMAGE_MARGIN * 2))

#define COLOR_TEXT white
#define COLOR_SMALLTEXT tsgl_color_raw(tsgl_color_fromHex(0x777777), framebuffer.colormode)

static tsgl_print_settings printsettings_text = {
    .locationMode = tsgl_print_start_top,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = ACHIEVEMENTS_ICON_TEXT_WIDTH,
    .targetHeight = ACHIEVEMENTS_ICON_TEXT_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,

    .contrast = 0.7
};

static tsgl_print_settings printsettings_smalltext = {
    .locationMode = tsgl_print_start_bottom,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = ACHIEVEMENTS_ICON_SMALL_TEXT_WIDTH,
    .targetHeight = ACHIEVEMENTS_ICON_SMALL_TEXT_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,

    .contrast = 0.7
};

static void draw_achievement(tsgl_pos x, tsgl_pos y, tsgl_pos index, bool completed, const char* image_path, const char* text, const char* smalltext) {
    tsgl_framebuffer_fill(&framebuffer, x, y, ACHIEVEMENTS_BLOCK_WIDTH, ACHIEVEMENTS_BLOCK_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, ACHIEVEMENTS_BLOCK_WIDTH, ACHIEVEMENTS_BLOCK_HEIGHT, white, ACHIEVEMENTS_BLOCK_BORDER_SIZE);

    tsgl_pos imageX = x + ACHIEVEMENTS_BLOCK_IMAGE_MARGIN;
    tsgl_pos imageY = y + ACHIEVEMENTS_BLOCK_IMAGE_MARGIN;

    if (completed) {
        tsgl_framebuffer_fill(&framebuffer, imageX, imageY, ACHIEVEMENTS_BLOCK_IMAGE_SIZE, ACHIEVEMENTS_BLOCK_IMAGE_SIZE, blue);
    }

    tsgl_sprite* sprite = gfx_loadSprite(image_path);
    if (sprite) {
        if (!completed) tsgl_sprite_apply_grayscale(sprite);
        PUSH_FUNC_TRANS(&framebuffer, imageX, imageY, sprite);
        tsgl_sprite_free(sprite);
    }

    tsgl_framebuffer_rect(&framebuffer, imageX, imageY, ACHIEVEMENTS_BLOCK_IMAGE_SIZE, ACHIEVEMENTS_BLOCK_IMAGE_SIZE, white, ACHIEVEMENTS_IMAGE_BORDER_SIZE);

    tsgl_pos textX = imageX + ACHIEVEMENTS_BLOCK_IMAGE_SIZE + ACHIEVEMENTS_BLOCK_IMAGE_MARGIN;
    tsgl_pos textY = imageY;
    tsgl_pos smalltextY = imageY + (ACHIEVEMENTS_BLOCK_IMAGE_SIZE - ACHIEVEMENTS_BLOCK_IMAGE_MARGIN);

    printsettings_text.fg = COLOR_TEXT;
    tsgl_framebuffer_text(&framebuffer, textX, textY, printsettings_text, text);

    printsettings_smalltext.fg = COLOR_SMALLTEXT;
    tsgl_framebuffer_text(&framebuffer, textX, smalltextY, printsettings_smalltext, smalltext);
    
}

void game_achievements_draw() {
    if (!current_state.achievements_opened) return;

    tsgl_pos x = (WIDTH / 2) - (ACHIEVEMENTS_WIDTH / 2);
    tsgl_pos y = (HEIGHT / 2) - (ACHIEVEMENTS_HEIGHT / 2);
    
    tsgl_framebuffer_fill(&framebuffer, x, y, ACHIEVEMENTS_WIDTH, ACHIEVEMENTS_HEIGHT, black);
    tsgl_framebuffer_rect(&framebuffer, x, y, ACHIEVEMENTS_WIDTH, ACHIEVEMENTS_HEIGHT, white, ACHIEVEMENTS_BORDER_SIZE);

    tsgl_pos offset_x = x + ACHIEVEMENTS_BLOCK_MARGIN;
    tsgl_pos offset_y = y + ACHIEVEMENTS_BLOCK_MARGIN;
    tsgl_pos offset_step = ACHIEVEMENTS_BLOCK_HEIGHT + ACHIEVEMENTS_BLOCK_MARGIN;

    //Дай поспать
    //10 часов
    draw_achievement(offset_x, offset_y, 0, current_state.achievements_completed_fullsleep, "/firmware/images/achivmnt/fullslep.bmp",
        "\xC4\xE0\xE9\x20\xEF\xEE\xF1\xEF\xE0\xF2\xFC",
        "\x31\x30\x20\xF7\xE0\xF1\xEE\xE2"
    );
    offset_y += offset_step;

    //Я богат
    //Заработать 1000
    draw_achievement(offset_x, offset_y, 1, current_state.achievements_completed_money, "/firmware/images/achivmnt/money.bmp",
        "\xDF\x20\xE1\xEE\xE3\xE0\xF2",
        "\xC7\xE0\xF0\xE0\xE1\xEE\xF2\xE0\xF2\xFC\x20\x31\x30\x30\x30"
    );
    offset_y += offset_step;

    //Задрот
    //Играть до 1000
    draw_achievement(offset_x, offset_y, 2, current_state.achievements_completed_gaming, "/firmware/images/achivmnt/gaming.bmp",
        "\xC7\xE0\xE4\xF0\xEE\xF2",
        "\xC8\xE3\xF0\xE0\xF2\xFC\x20\xE4\xEE\x20\x31\x30\x30\x30"
    );
    offset_y += offset_step;
}

void game_achievements_open() {
    pushsound_play("/firmware/sounds/bp_open.pcm", 16000, BACKPACK_SOUND_VOLUME);
    current_state.achievements_opened = true;
    game_updateActiveIcons();
}

void game_achievements_close() {
    pushsound_play("/firmware/sounds/bp_close.pcm", 16000, BACKPACK_SOUND_VOLUME);
    current_state.achievements_opened = false;
    game_updateActiveIcons();
}

void game_achievements_toggle() {
    if (current_state.achievements_opened) {
        game_achievements_close();
    } else {
        game_achievements_open();
    }
}
