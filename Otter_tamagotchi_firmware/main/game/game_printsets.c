#include "game_printsets.h"
#include "game_upmenu.h"

#define OVERLAY_TEXT_TARGET_WIDTH 9
#define OVERLAY_TEXT_TARGET_HEIGHT 9

tsgl_print_settings printsettings_overlay = {
    .locationMode = tsgl_print_start_top,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = OVERLAY_TEXT_TARGET_WIDTH,
    .targetHeight = OVERLAY_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,
    .stroke_thickness = 1,
    .stroke_no_clamp = true
};

#define SUBGAMES_TEXT_TARGET_WIDTH 8
#define SUBGAMES_TEXT_TARGET_HEIGHT 8

tsgl_print_settings printsettings_subgames = {
    .locationMode = tsgl_print_start_top,

    // multiline
    .multiline = true,
    .globalAlignmentX = true,
    .alignment = tsgl_print_alignment_center,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = SUBGAMES_TEXT_TARGET_WIDTH,
    .targetHeight = SUBGAMES_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR
};

#define GAMETITLE_TEXT_TARGET_WIDTH 16
#define GAMETITLE_TEXT_TARGET_HEIGHT 16

tsgl_print_settings printsettings_gametitle = {
    .locationMode = tsgl_print_start_top,

    // multiline
    .multiline = true,
    .globalAlignmentX = true,
    .alignment = tsgl_print_alignment_center,

    // font
    .font = DejaVuSerif,
    .localLocationMode = tsgl_print_localLocationMode_center,
    .targetWidth = GAMETITLE_TEXT_TARGET_WIDTH,
    .targetHeight = GAMETITLE_TEXT_TARGET_HEIGHT,

    .fill = TSGL_INVALID_RAWCOLOR,
    .bg = TSGL_INVALID_RAWCOLOR,
    .stroke_thickness = 1,
    .stroke_no_clamp = true
};