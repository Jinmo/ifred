# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("palette")

# One line only, not both
# include("ida-cmake/cmake/IDA.cmake")
# add_subdirectory ("palette/src/ida")
find_package(Qt5Widgets CONFIG REQUIRED)

add_subdirectory(palette/pybind11)

# Include sub-projects.
add_subdirectory ("palette")

enable_testing()
add_subdirectory ("benchmark")
