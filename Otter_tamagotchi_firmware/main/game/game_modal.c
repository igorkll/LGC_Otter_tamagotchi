#include "game_modal.h"
#include "game_printsets.h"

#define MODAL_WINDOW_MARGIN 20
#define MODAL_WINDOW_COLOR 

void game_modal_draw_gameover(int score, int maxscore) {
    char text[MAX_ACTION_LONG_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LONG_LEN, "GAMEOVER\nSCORE: %i\nMAX SCORE: %i", score, maxscore);

    printsettings_gametitle.fg = red;
    printsettings_gametitle.fg = red;
    printsettings_gametitle.width = WIDTH;
    printsettings_gametitle.height = HEIGHT;
    tsgl_framebuffer_text(&framebuffer, 0, 0, printsettings_gametitle, text);
}

void game_modal_draw_message(const char* message) {
    printsettings_gametitle.fg = red;
    printsettings_gametitle.fg = red;
    printsettings_gametitle.width = WIDTH - (MODAL_WINDOW_MARGIN * 2);
    printsettings_gametitle.height = HEIGHT - (MODAL_WINDOW_MARGIN * 2);
    tsgl_framebuffer_fill(&framebuffer, MODAL_WINDOW_MARGIN, MODAL_WINDOW_MARGIN, printsettings_gametitle.width, printsettings_gametitle.height, white);
    tsgl_framebuffer_text(&framebuffer, MODAL_WINDOW_MARGIN, MODAL_WINDOW_MARGIN, printsettings_gametitle, message);
}
