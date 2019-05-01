#include <widgets/palette_manager.h>

#include <bindings/pypalette.h>

PyPalette::PyPalette(const std::string &name, const std::string &placeholer, const py::list &entries) {
    QVector<Action> result;

    name_ = QString::fromStdString(name);
    placeholder_ = QString::fromStdString(placeholer);
    result.reserve(static_cast<int>(entries.size()));

    for (py::handle item : entries) {
        result.push_back(Action{QString::fromStdString(item.attr("id").cast<std::string>()),
                                QString::fromStdString(item.attr("description").cast<std::string>()),
                                QString::fromStdString(item.attr("shortcut").cast<std::string>())
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

    m.def("show_palette", [](PyPalette &palette) -> bool {
        show_palette(palette.name(), palette.placeholder(), palette.actions(), [](const Action &action) {
            py::gil_scoped_acquire gil;

            try {
                py::object trigger_action_py = py::module::import("__palette__").attr("execute_action");
                auto res = trigger_action_py(action.id.toStdString());

                return true;
            }
            catch (const std::runtime_error &error) {
                // This should not throw error
                auto write = py::module::import("sys").attr("stdout").attr("write");
                write(error.what());
                write("\n");
                PyErr_Clear();
            }

            return true;
        });
        return true;
    });

    m.attr("threading") = py::module::import("threading");

    py::exec(R"(
class Action:
	def __init__(self, id, description, handler):
		self.id = id
		self.description = description
		self.shortcut = ""
		self.handler = handler

def execute_action(id):
    # on main thread
    threading.Thread(target=__cur_palette__[id].handler, args=(__cur_palette__[id], )).run()

)", m.attr("__dict__"));

    m.attr("__version__") = "dev";
}

void init_python_module() {
    init__palette__();
}

