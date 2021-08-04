#include <palette/api.h>

#include <random>
#include <stdexcept>
#include <thread>
#include <algorithm>

#define COUNT 500000

QString random_key() {
  const char* keys[] = {"Shift"};
  return keys[rand() % (sizeof(keys) / sizeof(keys[0]))];
}

QVector<Action> testItems(int count=COUNT) {
  QVector<Action> action_list;

  action_list.reserve(count + 1);
  action_list.push_back(Action{"std::runtime_error", "raise exception!!", "",
                               "Just raises an exception"});

  for (int i = 0; i < count; i++) {
    auto id = QString::number(1LL * rand() * rand() * rand() * rand(), 36) +
              ":" + QString::number(i);
    action_list.push_back(Action{id, id + id,
                                 random_key() + "+" + QString::number(i % 10),
                                 QString::number(i) + "th element"});
  }

  return action_list;
}

class CustomService final : public SearchService {
  QVector<Action> items_;
 public:
  CustomService() : SearchService(nullptr), items_(testItems(500)) {
    connect(this, &SearchService::startSearching, this,
            &CustomService::onSearch);
  }

  void onSearch(QString keyword) {
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(items_.begin(), items_.end(), g);
    emit doneSearching(keyword, items_, 0);
    emit doneSearching(
        keyword, {Action{"install", "Press ENTER to install your extension."}},
        0);
  }

  void cancel() override {}
  bool runInSeparateThread() override { return true; }
};

QString TestPluginPath(const char* name) {
  // Don't worry! also packaged with bundle theme!
  // Just point a writable path
  return QString("./path_to_plugin_theme/") + name;
}

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  set_path_handler(TestPluginPath);
  QApplication::setQuitOnLastWindowClosed(true);

  CommandPalette palette;
  palette.setWindowFlags(Qt::Tool);
  palette.setAttribute(
      Qt::WA_TranslucentBackground, false);  // enable MainWindow to be transparent
//  palette.show("test dynamic palette", "Enter item name (dynamic service) ...",
//               new CustomService(), "Ctrl+P",
//               [](Action& action) { qDebug() << action.id << action.name; return false; });
    palette.show("<test palette>", "Enter item name...", testItems(),
    "Ctrl+P",
                  [](Action& action) {
                    if (action.id == "std::runtime_error") {
                      throw std::runtime_error("raised!");
                    }
                    qDebug() << action.id << action.name << action.shortcut;
                    return false;
                  });

  QApplication::exec();
}
