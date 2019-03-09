#define IMPORTING_PALETTE

#include <widgets/qpalette_inner.h>
#include <widgets/palette_manager.h>

#include <QApplication>
#include <string>

#define COUNT 20000
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
		auto* source = entries().source();

		source->setRowCount(static_cast<int>(COUNT));
		source->setColumnCount(3);

		int i = 0;
		for (i = 0; i < COUNT; i++) {
			auto id = get_random_str();

			source->setData(source->index(i, 0), get_random_str());
			source->setData(source->index(i, 1), "");
			source->setData(source->index(i, 2), id);
		}
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
