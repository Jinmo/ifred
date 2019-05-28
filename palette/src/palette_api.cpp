#include <palette_api.h>
#include <widgets/palette.h>
#include <action.h>
#include <utility>

CommandPalette* g_current_widget;
pathhandler_t pluginPath;

template <typename F>
static void postToThread2(F&& fun, QThread* thread = qApp->thread()) {
    QObject* obj = QAbstractEventDispatcher::instance(thread);
    Q_ASSERT(obj);
    QObject src;
    QObject::connect(&src, &QObject::destroyed, obj, std::forward<F>(fun),
        Qt::QueuedConnection);
}

void show_palette(const QString& name, const QString& placeholder, const QVector<Action>& actions, const QString& closeKey, ActionHandler func) {
    postToThread2([=]() {
        g_current_widget = new CommandPalette();
        g_current_widget->show(name, placeholder, actions, closeKey, std::move(func));
        });
}

void cleanup_palettes() {
    Q_CLEANUP_RESOURCE(theme_bundle);
}

void set_path_handler(pathhandler_t handler) {
    pluginPath = handler;
}
