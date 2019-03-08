include("ida-cmake/cmake/IDA.cmake")

set(sources src/ida.cpp
			src/myfilter.cpp
			src/qsearch.cpp
			src/utils.cpp
			src/qpalette_inner.cpp
			src/qitems.cpp
			src/qitem.cpp
			src/bindings/pypalette.cpp
			src/bindings/qpypalette_inner.cpp
			src/palette_manager.cpp
)

set(PYTHON_EXECUTABLE       ""    CACHE PATH "Python executable (x64 if >= 7.0)")
set(IDA_SKIP_ADD_LIBRARY ON)
add_subdirectory(pybind11)

pybind11_add_module(${CMAKE_PROJECT_NAME} ${sources})
add_ida_qt_plugin(${CMAKE_PROJECT_NAME})

# TODO: Add tests and install targets if needed.
