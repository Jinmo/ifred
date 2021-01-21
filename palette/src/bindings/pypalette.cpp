#include <palette_api.h>
#include <bindings/pypalette.h>

PyPalette::PyPalette(const std::string& name, const std::string& placeholer, const py::list& entries) {
    QVector<Action> result;

    name_ = QString::fromStdString(name);
    placeholder_ = QString::fromStdString(placeholer);
    result.reserve(static_cast<int>(entries.size()));

    for (py::handle item : entries) {
        result.push_back(Action{
            QString::fromStdString(item.attr("id").cast<std::string>()),
            QString::fromStdString(item.attr("name").cast<std::string>()),
            QString::fromStdString(item.attr("shortcut").cast<std::string>()),
            QString::fromStdString(item.attr("description").cast<std::string>())
            });
    }

    py::object scope = py::module::import("__palette__").attr("__dict__");

    scope["__entries__"] = entries;

    py::exec(
        "__cur_palette__ = {l.id: l for l in __entries__}",
        scope);

    actions_.swap(result);
}

PYBIND11_MODULE(__palette__, m) {
    m.doc() = R"()";

    py::class_<PyPalette>(m, "Palette")
        .def(py::init<std::string, std::string, py::list>());

    m.def("show_palette", [](PyPalette & palette, std::string closeKey) -> bool {
        show_palette(palette.name(), palette.placeholder(), palette.actions(), QString::fromStdString(closeKey), [](const Action & action) {
            py::gil_scoped_acquire gil;

            try {
                py::object trigger_action_py = py::module::import("__palette__").attr("execute_action");
                auto res = trigger_action_py(action.id.toStdString());

                return true;
            }
            catch (const std::runtime_error & error) {
                // This should not throw error
                auto write = py::module::import("sys").attr("stdout").attr("write");
                write(error.what());
                write("\n");
                PyErr_Clear();
            }

            return true;
            });
        return true;
        }, py::arg("palette"), py::arg("close_key") = "");

    m.attr("threading") = py::module::import("threading");
    m.attr("threading") = py::module::import("threading");

    auto globals = m.attr("__dict__");
    PyDict_SetItemString(globals.ptr(), "__builtins__", PyEval_GetBuiltins());

    py::exec(R"(

class Action:
    def __init__(self, id, name, handler, shortcut="", description=""):
        self.id = id
        self.name = name
        self.shortcut = shortcut
        self.description = description
        self.handler = handler

def execute_action(id):
    # on main thread
    threading.Thread(target=__cur_palette__[id].handler, args=(__cur_palette__[id], )).run()

)", globals);

    m.attr("__version__") = "dev";
}

void init_python_module() {
#if PY_MAJOR_VERSION >= 3 /// Compatibility macros for various Python versions
    auto ptr = PyInit___palette__();
    py::str str("__palette__");
    _PyImport_FixupExtensionObject(
        ptr,
        str.ptr(),
        str.ptr()
#if PY_MINOR_VERSION >= 7
        // Python 3.6 uses this by default
        , PyImport_GetModuleDict()
#endif
    );
#else
    init__palette__();
#endif
}

