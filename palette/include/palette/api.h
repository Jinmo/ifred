#pragma once

#include <palette_export.h>
#include <palette/utils.h>
#include <palette/widgets/palette.h>
#include <palette/action.h>

void PALETTE_EXPORT show_palette(const QString& name, const QString& placeholder, const QVector<Action>& actions, const QString& closeKey, ActionHandler func);
void PALETTE_EXPORT cleanup_palettes();
void PALETTE_EXPORT set_path_handler(pathhandler_t handler);
