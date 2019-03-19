#pragma once

#include <QtGui>
#include <QtWidgets>
#include <string>

#define NDEBUG
#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <bindings/qpypalette_inner.h>

class PyPalette {
	QPyPaletteInner* inner_;
public:
	PyPalette(std::string &name, py::list entries);

	EnterResult trigger_action(Action action);

	QPaletteInner* inner() { return inner_; }
};

