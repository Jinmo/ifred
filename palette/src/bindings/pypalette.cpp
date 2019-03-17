#include <widgets/qpalette_inner.h>
#include <widgets/palette_manager.h>

#include <bindings/qpypalette_inner.h>
#include <bindings/pypalette.h>

PYBIND11_MODULE(__palette__, m) {
    m.doc() = R"()";

    py::class_<PyPalette>(m, "Palette")
        .def(py::init<py::list>());

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