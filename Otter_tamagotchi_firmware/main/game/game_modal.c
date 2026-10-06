#include "game_modal.h"
#include "game_printsets.h"

#define GAMEOVER_COLOR red
#define GAMEOVER_STROKE tsgl_color_raw(tsgl_color_pack(75, 0, 0), framebuffer.colormode)

#define WIN_COLOR green
#define WIN_STROKE tsgl_color_raw(tsgl_color_pack(0, 75, 0), framebuffer.colormode)

#define MODAL_WINDOW_TEXT_MARGIN 5
#define MODAL_WINDOW_MARGIN_X 10
#define MODAL_WINDOW_MARGIN_Y 30
#define MODAL_WINDOW_TEXT_COLOR tsgl_color_raw(tsgl_color_pack(0, 0, 0), framebuffer.colormode)
#define MODAL_WINDOW_BG_COLOR tsgl_color_raw(tsgl_color_pack(199, 199, 199), framebuffer.colormode)
#define MODAL_WINDOW_BORDER_COLOR tsgl_color_raw(tsgl_color_pack(0, 0, 64), framebuffer.colormode)
#define MODAL_WINDOW_BORDER_STROKE 2

void game_modal_draw_gameover(int score, int maxscore) {
    char text[MAX_ACTION_LONG_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LONG_LEN, "GAMEOVER\nSCORE: %i\nMAX SCORE: %i", score, maxscore);

    printsettings_gametitle.fg = GAMEOVER_COLOR;
    printsettings_gametitle.stroke = GAMEOVER_STROKE;
    printsettings_gametitle.width = WIDTH;
    printsettings_gametitle.height = HEIGHT;
    tsgl_framebuffer_text(&framebuffer, 0, 0, printsettings_gametitle, text);
}

void game_modal_draw_win(const char* message) {
    char text[MAX_ACTION_LONG_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LONG_LEN, "WIN!\nSCORE: %i\nMAX SCORE: %i", score, maxscore);

    printsettings_gametitle.fg = WIN_COLOR;
    printsettings_gametitle.stroke = WIN_STROKE;
    printsettings_gametitle.width = WIDTH;
    printsettings_gametitle.height = HEIGHT;
    tsgl_framebuffer_text(&framebuffer, 0, 0, printsettings_gametitle, text);
}

void game_modal_draw_message(const char* message) {
    tsgl_pos width = WIDTH - (MODAL_WINDOW_MARGIN_X * 2);
    tsgl_pos height = HEIGHT - (MODAL_WINDOW_MARGIN_Y * 2);
    tsgl_framebuffer_fill(&framebuffer, MODAL_WINDOW_MARGIN_X, MODAL_WINDOW_MARGIN_Y, width, height, MODAL_WINDOW_BG_COLOR);
    tsgl_framebuffer_rect(&framebuffer, MODAL_WINDOW_MARGIN_X, MODAL_WINDOW_MARGIN_Y, width, height, MODAL_WINDOW_BORDER_COLOR, MODAL_WINDOW_BORDER_STROKE);

    printsettings_message.fg = MODAL_WINDOW_TEXT_COLOR;
    printsettings_message.width = width - (MODAL_WINDOW_TEXT_MARGIN * 2);
    printsettings_message.height = height - (MODAL_WINDOW_TEXT_MARGIN * 2);
    tsgl_framebuffer_text(&framebuffer, MODAL_WINDOW_MARGIN_X + MODAL_WINDOW_TEXT_MARGIN, MODAL_WINDOW_MARGIN_Y + MODAL_WINDOW_TEXT_MARGIN, printsettings_message, message);
}
