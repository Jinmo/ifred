#pragma once

#include <widgets/qpalette_inner.h>

class PyPalette;

class QPyPaletteInner : public QPaletteInner {
	PyPalette* python_side_;
public:
	QPyPaletteInner(PyPalette* python_side, const QVector<Action> &items) : QPaletteInner(nullptr, std::move(items)), python_side_(python_side) {}

	EnterResult enter_callback(Action &action) override;
};

