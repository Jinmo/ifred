#include <widgets/qpalette_inner.h>
#include <widgets/palette_manager.h>

#include <bindings/qpypalette_inner.h>
#include <bindings/pypalette.h>

PYBIND11_MODULE(__palette__, m) {
	m.doc() = R"()";

	py::class_<PyPalette>(m, "Palette")
		.def(py::init<std::vector<PyAction>&>());

	py::class_<PyAction>(m, "Action")
		.def(py::init<std::string &, std::string &, std::string &, py::object &>(), py::arg("id"), py::arg("description"), py::arg("shortcut"), py::arg("handler"))
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