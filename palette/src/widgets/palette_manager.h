#pragma once

#include "qpalette_inner.h"
#include "utils.h"

void PALETTE_EXPORT show_palette(QPaletteInner* inner);
void PALETTE_EXPORT cleanup_palettes();
void PALETTE_EXPORT set_path_handler(pathhandler_t handler);
