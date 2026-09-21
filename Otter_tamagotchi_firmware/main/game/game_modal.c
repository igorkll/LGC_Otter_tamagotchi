#include "game_modal.h"
#include "game_printsets.h"

void game_modal_draw_gameover(int score, int maxscore) {
    char text[MAX_ACTION_LONG_LEN];
    TSGL_funcs_slnprintf(text, MAX_ACTION_LONG_LEN, "GAMEOVER\nSCORE: %i\nMAX SCORE: %i", score, maxscore);

    printsettings_gametitle.fg = red;
    printsettings_gametitle.fg = red;
    printsettings_gametitle.width = WIDTH;
    printsettings_gametitle.height = HEIGHT;
    tsgl_framebuffer_text(&framebuffer, 0, 0, printsettings_gametitle, text);
}
