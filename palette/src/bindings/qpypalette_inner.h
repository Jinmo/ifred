#pragma once

#include "../qpalette_inner.h"

class PyPalette;

class QPyPaletteInner : public QPaletteInner {
	PyPalette* python_side_;
public:
	QPyPaletteInner(PyPalette* python_side) : QPaletteInner(nullptr, nullptr), python_side_(python_side) {}

	EnterResult enter_callback() override;
};

