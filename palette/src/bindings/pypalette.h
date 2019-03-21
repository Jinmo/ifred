#pragma once

#include <QtGui>
#include <QtWidgets>
#include <string>

#undef NDEBUG
#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/stl.h>

namespace py = pybind11;

class PyPalette {
    QVector<Action> actions_;
    QString name_;
public:
	PyPalette(const std::string &name, py::list entries);

    const QVector<Action>& actions() { return actions_; }
    const QString& name() { return name_; }
};

