#include <thread>
#include <stdexcept>

#include <palette/api.h>

#define COUNT 500000

QString random_key() {
    const char* keys[] = { "Shift" };
    return keys[rand() % (sizeof(keys) / sizeof(keys[0]))];
}

QVector<Action> testItems() {
    QVector<Action> action_list;

    action_list.reserve(COUNT + 1);
    action_list.push_back(Action{ "std::runtime_error", "raise exception!!", "", "Just raises an exception" });

    for (int i = 0; i < COUNT; i++) {
        auto id = QString::number(1LL * rand() * rand() * rand() * rand(), 36) + ":" + QString::number(i);
        action_list.push_back(Action{ id, id + id, random_key() + "+" + QString::number(i % 10), QString::number(i) + "th element" });
    }

    return action_list;
}

const QString TestPluginPath(const char* name) {
    // Don't worry! also packaged with bundle theme!
    // Just point a writable path
    return QString("./path_to_plugin_theme/") + name;
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    set_path_handler(TestPluginPath);

    auto palette = new CommandPalette(nullptr);
    palette->show("<test palette>", "Enter item name...", testItems(), "Ctrl+P", [](Action& action) {
        if (action.id == "std::runtime_error") {
            throw std::runtime_error("raised!");
        }
        qDebug() << action.id << action.name << action.shortcut;
        return false;
        });

    app.exec();
}
