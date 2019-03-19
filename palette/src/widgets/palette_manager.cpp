#include <widgets/palette_manager.h>
#include <widgets/qpalettecontainer.h>

QPaletteContainer* g_current_widget;
pathhandler_t pluginPath;

void cleanup_palettes() {
    if (g_current_widget) {
        delete g_current_widget;
        g_current_widget = nullptr;
    }

    Q_CLEANUP_RESOURCE(theme_bundle);
}

void set_path_handler(pathhandler_t handler) {
    pluginPath = handler;
}