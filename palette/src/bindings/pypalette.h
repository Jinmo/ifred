#pragma once

#include <QtGui>
#include <QtWidgets>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <bindings/qpypalette_inner.h>

class PyAction : public Action {
public:
	PyAction() : Action() {}
	PyAction(std::string &id, std::string &description, std::string &shortcut, py::object& handler)
		: Action(QString::fromStdString(id), QString::fromStdString(description), QString::fromStdString(shortcut)),
		handler_(py::reinterpret_borrow<py::object>(handler)) {}
	py::object handler_;
};

static QString py_to_qstring(py::object obj) {
	std::string resStr = py::str(obj);
	return QString(resStr.c_str());
}

class PyPalette {
	QPyPaletteInner* inner_;
	py::dict action_map_;
public:
	PyPalette(const std::vector<PyAction> entries)
		: inner_(new QPyPaletteInner(this, std::move(populateList(std::move(entries))))) {
	}

	const QVector<Action>& populateList(const std::vector<PyAction>& entries) const {
		QVector<Action> result;

		result.reserve(entries.size());
		for (auto& item : entries) {
			action_map_[py::str(item.id().toStdString())] = py::reinterpret_borrow<py::object>(item.handler_);
			result.push_back((Action)item);
		}

		return result;
	}

	EnterResult trigger_action(Action& action) {
		py::object handler = action_map_[py::str(action.id().toStdString())];
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

