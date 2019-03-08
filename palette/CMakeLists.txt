# CMakeList.txt : CMake project for palette, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(palette)

include("ida-cmake/cmake/IDA.cmake")

set(sources src/ida.cpp
			src/myfilter.cpp
			src/qsearch.cpp
			src/utils.cpp
			src/qpalette_inner.cpp
			src/qitems.cpp
			src/qitem.cpp
			src/python.cpp
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
add_subdirectory(pybind11)
add_ida_qt_plugin(${CMAKE_PROJECT_NAME} ${sources})
pybind11_add_module(${CMAKE_PROJECT_NAME} ${sources})

# TODO: Add tests and install targets if needed.
