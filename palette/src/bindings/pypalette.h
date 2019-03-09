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

class PaletteItem {
	std::string description_, id_, shortcut_;
	py::function handler_;

public:
	PaletteItem() {}
	PaletteItem(std::string id, py::object handler, std::string description, std::string shortcut)
		: description_(description), id_(id), shortcut_(shortcut), handler_(handler) {
	}

	py::function handler() { return handler_; }
	void set_handler(py::object& new_handler) { handler_ = new_handler; }

	std::string id() { return id_; }
};

class PyPalette {
	py::list entries_;
	QPyPaletteInner* inner_;
	QHash<QString, PaletteItem> action_map_;
public:
	PyPalette(py::list& entries) : entries_(entries), inner_(new QPyPaletteInner(this)) {
		try {
			populateList();
		}
		catch (pybind11::key_error) {
			delete inner_;
			inner_ = NULL;
		}
	}

	void populateList() {
		auto source = inner_->entries().source();

		source->setRowCount(static_cast<int>(entries_.size()));
		source->setColumnCount(3);

		int i = 0;
		for (auto item : entries_) {
			auto id = py_to_qstring(item["id"]);
			py::function handler = item["handler"];

			source->setData(source->index(i, 0), py_to_qstring(item["description"]));
			source->setData(source->index(i, 1), py_to_qstring(item["shortcut"]));
			source->setData(source->index(i, 2), id);

			action_map_[id] = PaletteItem(item["id"].str(), handler, item["description"].str(), item["shortcut"].str());
			i += 1;
		}
	}

	EnterResult trigger_action(QString& id) {
		py::object handler = action_map_[id].handler();
		qDebug() << QString(py::cast<std::string>(handler).c_str());
		return true;
		py::object& res = handler(action_map_[id]);

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

