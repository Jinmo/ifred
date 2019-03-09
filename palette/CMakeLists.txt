cmake_minimum_required(VERSION 3.1)
include("ida-cmake/cmake/IDA.cmake")
include(GenerateExportHeader)

set(sources src/ida/plugin.cpp
			src/widgets/myfilter.cpp
			src/widgets/qsearch.cpp
			src/utils.cpp
			src/widgets/qpalette_inner.cpp
			src/widgets/qitems.cpp
			src/widgets/qitem.cpp
			src/widgets/palette_manager.cpp
			src/bindings/pypalette.cpp
			src/bindings/qpypalette_inner.cpp
)

set(PYTHON_EXECUTABLE "" CACHE PATH "Python executable (x64 if >= 7.0)")
set(IDA_SKIP_ADD_LIBRARY ON)

pybind11_add_module(palette SHARED ${sources})
add_ida_qt_plugin(palette)

target_include_directories(palette PUBLIC src)
GENERATE_EXPORT_HEADER(palette)

# TODO: Add tests and install targets if needed.
