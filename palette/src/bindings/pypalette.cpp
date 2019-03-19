#include <widgets/qpalette_inner.h>
#include <widgets/palette_manager.h>

#include <bindings/qpypalette_inner.h>
#include <bindings/pypalette.h>

static QString py_to_qstring(const std::string &obj) {
    return QString::fromStdString(obj);
}

EnterResult PyPalette::trigger_action(Action action) {
    py::gil_scoped_acquire gil;

    try {
        py::object action_map = py::module::import("__main__").attr("__cur_palette__");
        auto actionpy = action_map[py::str(action.id().toStdString())];

        auto res = actionpy.attr("handler")(actionpy);

        if (py::isinstance<EnterResult>(res)) {
            EnterResult enter_result = py::cast<EnterResult>(res);
            return enter_result;
        }
        else {
            return EnterResult(res == Py_True);
        }
    }
    catch (const std::runtime_error & error) {
        // This should not throw error
        auto write = py::module::import("sys").attr("stdout").attr("write");
        write(error.what());
        write("\n");
        PyErr_Clear();
    }
}

PyPalette::PyPalette(std::string& name, py::list entries) {
    QVector<Action> result;

    result.reserve(static_cast<int>(entries.size()));

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

    inner_ = (new QPyPaletteInner(this, QString::fromStdString(name), std::move(result)));
}

PYBIND11_MODULE(__palette__, m) {
    m.doc() = R"()";

    py::class_<PyPalette>(m, "Palette")
        .def(py::init<std::string, py::list>());

    py::exec(R"(
class Action:
	def __init__(self, id, description, handler):
		self.id = id
		self.description = description
		self.shortcut = ""
		self.handler = handler
)", m.attr("__dict__"));

    m.def("show_palette", [](PyPalette & palette) {
        show_palette(palette.inner());
        });

    m.attr("__version__") = "dev";
}

void init_python_module() {
    init__palette__();
}

