#define IMPORTING_PALETTE

#include <widgets/palette_manager.h>

#include <QApplication>
#include <string>

#define COUNT 200000
#define LENGTH 20

const uchar key[] = "abcdefghjiklmnopqrstuvwxyz";

QString get_random_str() {
    QString result;
    result.resize(LENGTH);
    for (int i = 0; i < LENGTH; i++)
        result[i] = key[rand() % (sizeof(key) - 1)];
    return result;
}

QVector<Action> testItems() {
    QVector<Action> action_list;

    action_list.reserve(COUNT + 1);

    action_list.push_back(Action("std::runtime_error", "raise exception", ""));

    for (int i = 0; i < COUNT; i++) {
        auto id = get_random_str();
        action_list.push_back(Action(id, get_random_str(), ""));
    }

    return action_list;
}

const QString TestPluginPath(const char* name) {
    return QDir::homePath() + "/palette_test/" + name;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    set_path_handler(TestPluginPath);

    show_palette("<test palette>", testItems(), [](const Action & action) {
        if (action.id() == "std::runtime_error") {
            throw std::runtime_error("raised!");
        }
        qDebug() << action.id() << action.description() << action.shortcut();

        return false;
        });
    return app.exec();
}
