#include "../widgets/qpalette_inner.h"
#include "../widgets/palette_manager.h"
#include "pypalette.h"
#include "qpypalette_inner.h"

PYBIND11_MODULE(__palette__, m) {
	m.doc() = R"()";

	py::class_<PyPalette>(m, "Palette")
		.def(py::init<py::list&>());

	py::class_<Action>(m, "Action")
		.def_property_readonly("id", &Action::id)
		.def_property_readonly("shortcut", &Action::shortcut)
		.def_property_readonly("description", &Action::description);

	m.def("show_palette", [](PyPalette & palette) {
		show_palette(palette.inner());
		});

	m.attr("__version__") = "dev";
}

void init_python_module() {
	init__palette__();
}