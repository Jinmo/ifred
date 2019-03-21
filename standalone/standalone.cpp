#include <widgets/palette_manager.h>
#define COUNT 100

QVector<Action> testItems() {
    QVector<Action> action_list;

    action_list.reserve(COUNT + 1);
    action_list.push_back(Action("std::runtime_error", "raise exception", ""));

    for (int i = 0; i < COUNT; i++) {
        auto id = QString::number(rand());
        action_list.push_back(Action(id, id, ""));
    }

    return action_list;
}

const QString TestPluginPath(const char* name) {
    // Don't worry! also packaged with bundle theme!
    // Just point a writable path
    return QString("./path_to_plugin_theme/") + name;
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    set_path_handler(TestPluginPath);

    show_palette("<test palette>", testItems(), [](const Action & action) {
        if (action.id() == "std::runtime_error") {
            throw std::runtime_error("raised!");
        }
        qDebug() << action.id() << action.description() << action.shortcut();
        return false;
        });
}