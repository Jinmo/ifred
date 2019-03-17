#include <widgets/palette_manager.h>
#include <widgets/qpalettecontainer.h>

QPaletteContainer* g_current_widget;
pathhandler_t pluginPath;

void show_palette(QPaletteInner* inner) {
    if (!g_current_widget)
        g_current_widget = new QPaletteContainer();

    g_current_widget->clear();
    g_current_widget->add(inner);
    g_current_widget->show();
    g_current_widget->showWidget(0);
}

void cleanup_palettes() {
    if (g_current_widget) {
        delete g_current_widget;
        g_current_widget = NULL;
    }

    Q_CLEANUP_RESOURCE(theme_bundle);
}

void set_path_handler(pathhandler_t handler) {
    pluginPath = handler;
}