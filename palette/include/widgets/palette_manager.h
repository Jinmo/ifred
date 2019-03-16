#pragma once

#include <utils.h>
#include <palette_export.h>

class QPaletteInner;

void PALETTE_EXPORT show_palette(QPaletteInner* inner);
void PALETTE_EXPORT cleanup_palettes();
void PALETTE_EXPORT set_path_handler(pathhandler_t handler);
