#pragma once

#include <QtGui>
#include <QtWidgets>
#include <string>

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include <bindings/qpypalette_inner.h>

static QString py_to_qstring(py::object obj) {
	std::string resStr = py::str(obj);
	return QString(resStr.c_str());
}

class PyPalette {
	py::list entries_;
	QPyPaletteInner* inner_;
	QHash<QString, py::object> action_map_;
public:
	PyPalette(py::list& entries)
		: entries_(entries), inner_(new QPyPaletteInner(this, populateList(entries))) {
	}

	QVector<Action> populateList(py::list entries) const {
		QVector<Action> result;
		int i = 0;

		result.reserve(entries.size());
		for (auto item : entries_) {
			auto id = py_to_qstring(item["id"]);
			py::object handler = item["handler"];

			result.push_back(Action(id, py_to_qstring(item["description"]), py_to_qstring(item["shortcut"])));

			//action_map_[id] = handler;
			i += 1;
		}

		return result;
	}

	EnterResult trigger_action(Action &action) {
		py::object handler = action_map_[action.id()]();
		py::object& res = handler(action);

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

