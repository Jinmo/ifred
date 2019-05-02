#include <widgets/palette_manager.h>
#include <widgets/qpalettecontainer.h>
#include <action.h>
#include <utility>

QPaletteContainer* g_current_widget;
pathhandler_t pluginPath;

void show_palette(const QString& name, const QString& placeholder, const QVector<Action>& actions, const QString& closeKey, ActionHandler func) {
    if (!g_current_widget) {
        g_current_widget = new QPaletteContainer();
    }
    emit g_current_widget->show(name, placeholder, actions, closeKey, std::move(func));
}

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
