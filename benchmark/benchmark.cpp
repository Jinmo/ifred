#define IMPORTING_PALETTE

#include <widgets/qpalette_inner.h>
#include <widgets/palette_manager.h>

#include <QApplication>
#include <string>

#define COUNT 100000
#define LENGTH 20

const char key[] = "abcdefghjiklmnopqrstuvwxyz";

QString get_random_str() {
	QString result;
	result.resize(LENGTH);
	for (int i = 0; i < LENGTH; i++)
		result[i] = key[rand() % (sizeof(key) - 1)];
	return result;
}

class TestPalette : public QPaletteInner {
public:
	TestPalette() : QPaletteInner(nullptr, nullptr) {
		populateList();
	}
	void populateList() {
		QVector<Action> action_list;

		action_list.reserve(COUNT);

		for (int i = 0; i < COUNT; i++) {
			auto id = get_random_str();
			action_list.push_back(Action(id, get_random_str(), ""));
		}

		qDebug() << action_list.size();

		entries().model()->populate(action_list);
	}

	EnterResult enter_callback() override {
		return true;
	}
};

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	auto palette = new TestPalette();
	show_palette(palette);
	return app.exec();
}
