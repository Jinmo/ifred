#pragma once

#include <QtGui>
#include <QtWidgets>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <bindings/qpypalette_inner.h>

static QString py_to_qstring(const std::string obj) {
	return QString::fromStdString(obj);
}

class PyPalette {
	QPyPaletteInner* inner_;
public:
	PyPalette(py::list entries) {
		QVector<Action> result;

		result.reserve(entries.size());

		for (int i = 0; i < entries.size(); i++) {
			py::handle item = entries[i];
			result.push_back(Action(
				py_to_qstring(item.attr("id").cast<std::string>()),
				py_to_qstring(item.attr("description").cast<std::string>()),
				py_to_qstring(item.attr("shortcut").cast<std::string>())
			));
		}

		py::object scope = py::module::import("__main__").attr("__dict__");

		scope["__entries__"] = entries;

		py::exec(
			"__cur_palette__ = {l.id: l for l in __entries__}",
			scope);

		inner_ = (new QPyPaletteInner(this, std::move(result)));
	}

	EnterResult trigger_action(Action action) {
		py::gil_scoped_acquire gil;
		py::object action_map = py::module::import("__main__").attr("__cur_palette__");
		auto actionpy = action_map[py::str(action.id().toStdString())];

		auto res = actionpy.attr("handler")(actionpy);

		if (py::isinstance<EnterResult>(res)) {
			EnterResult enter_result = py::cast<EnterResult>(res);
			return enter_result;
		}
		else {
			return EnterResult(py::cast<bool>(res));
		}
	}

	QPaletteInner* inner() { return inner_; }
};

