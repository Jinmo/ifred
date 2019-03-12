cmake_minimum_required(VERSION 3.1)
include("ida-cmake/cmake/IDA.cmake")
include(GenerateExportHeader)

set(sources src/widgets/myfilter.cpp
			src/widgets/qsearch.cpp
			src/utils.cpp
			src/widgets/qpalette_inner.cpp
			src/widgets/qitems.cpp
			src/widgets/qitem.cpp
			src/widgets/palette_manager.cpp
			src/bindings/pypalette.cpp
			src/bindings/qpypalette_inner.cpp
)

set(sources_ida src/ida/plugin.cpp)

set(PYTHON_EXECUTABLE "" CACHE PATH "Python executable (x64 if >= 7.0)")

pybind11_add_module(palette STATIC ${sources})

target_include_directories(palette PUBLIC src)
GENERATE_EXPORT_HEADER(palette)

set(IDA_SKIP_ADD_LIBRARY ON)
pybind11_add_module(ida_palette SHARED ${sources_ida})
add_ida_qt_plugin(ida_palette)
target_link_libraries(ida_palette PUBLIC palette)

# TODO: Add tests and install targets if needed.
