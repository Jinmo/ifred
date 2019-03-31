#pragma once

#include <utils.h>
#include <palette_export.h>
#include <widgets/qpalettecontainer.h>
#include <action.h>

class QPaletteInner;

extern QPaletteContainer* g_current_widget;

void show_palette(const QString& name, const QString &placeholder, const QVector<Action>& actions, ActionHandler func);

void PALETTE_EXPORT cleanup_palettes();
void PALETTE_EXPORT set_path_handler(pathhandler_t handler);
