#pragma once

#include <utils.h>
#include <palette_export.h>
#include <widgets/qpalettecontainer.h>
#include <action.h>

#include <functional>

class QPaletteInner;

extern QPaletteContainer* g_current_widget;

static void show_palette(const QString& name, const QVector<Action>& actions, ActionHandler func) {
    if (!g_current_widget) {
        g_current_widget = new QPaletteContainer();
    }
    emit g_current_widget->show(name, actions, func);
}

void PALETTE_EXPORT cleanup_palettes();
void PALETTE_EXPORT set_path_handler(pathhandler_t handler);
