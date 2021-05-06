#pragma once

#include <QtGui>
#include <QtWidgets>
#include <string>

#undef NDEBUG
#pragma push_macro("slots")
#undef slots
#include <pybind11/eval.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#pragma pop_macro("slots")

namespace py = pybind11;

class PyPalette {
  QVector<Action> actions_;
  QString name_;
  QString placeholder_;

 public:
  PyPalette(const std::string& name, const std::string& placeholder,
            const py::list& entries);

  const QVector<Action>& actions() { return actions_; }
  const QString& name() { return name_; }
  const QString& placeholder() { return placeholder_; }
};
