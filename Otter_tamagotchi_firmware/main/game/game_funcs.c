#include "game_funcs.h"
#include "game.h"

static tsgl_print_settings printsettings_anystatus_title = {
    .multiline = true,
    .locationMode = tsgl_print_start_top,
    .height = ANYSTATUS_HEIGHT,
    .globalAlignmentX = tsgl_print_alignment_center,
    .globalAlignmentY = tsgl_print_alignment_center,
    .alignment = tsgl_print_alignment_center,
    
    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = ANYSTATUS_TEXT_TARGET_WIDTH,
    .targetHeight = ANYSTATUS_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

void game_funcs_drawAnyStatus(tsgl_pos positionY, int state, int stateMin, int stateMax, const char* text, bool vflip) {
    tsgl_pos sizeX = WIDTH * ANYSTATUS_SIZE_MUL;
    tsgl_pos sizeY = ANYSTATUS_HEIGHT;
    tsgl_pos fillSize = tsgl_math_imap(state, stateMin, stateMax, 0, sizeX - (ANYSTATUS_LINE_PADDING * 2));
    
    tsgl_pos positionX = (WIDTH / 2) - (sizeX / 2);
    tsgl_pos positionY_line = positionY - ANYSTATUS_OBJ_OFFSET;
    tsgl_pos positionY_text = positionY + ANYSTATUS_OBJ_OFFSET;

    if (vflip) {
        tsgl_pos _old = positionY_line;
        positionY_line = positionY_text;
        positionY_text = _old;
    }

    tsgl_framebuffer_rect(&framebuffer, positionX, positionY_line, sizeX, sizeY, red, 2);
    tsgl_framebuffer_fill(&framebuffer, positionX + ANYSTATUS_LINE_PADDING, positionY_line + ANYSTATUS_LINE_PADDING, fillSize, sizeY - (ANYSTATUS_LINE_PADDING * 2), red);

    printsettings_anystatus_title.width = sizeX;
    printsettings_anystatus_title.fg = red;
    tsgl_framebuffer_text(&framebuffer, positionX, positionY_text, printsettings_anystatus_title, text);
}