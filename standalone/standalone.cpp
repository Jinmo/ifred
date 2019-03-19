#define IMPORTING_PALETTE

#include <widgets/qpalette_inner.h>
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

class TestPalette : public QPaletteInner {
public:
	TestPalette() : QPaletteInner(nullptr, "<test palette>", std::move(testItems())) {
	}

	EnterResult enter_callback(Action &action) override {
        if (action.id() == "std::runtime_error") {
            throw std::runtime_error("raised!");
        }
		qDebug() << action.id() << action.description() << action.shortcut();

		return false;
	}
};

const QString TestPluginPath(const char* name) {
	return QDir::homePath() + "/palette_test/" + name;
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	set_path_handler(TestPluginPath);

	auto palette = new TestPalette();
	show_palette(palette);
	return app.exec();
}
