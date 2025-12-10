# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_ARCHITECTURES x86_64)

project ("palette")
set(BUILD_EXAMPLES OFF CACHE BOOL "")

# Prevent CMake from automatically adding rpaths
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# Prevent full paths from being embedded in binaries
# Remap source paths to relative paths in debug info
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    get_filename_component(PROJECT_SOURCE_DIR_ABS "${CMAKE_SOURCE_DIR}" ABSOLUTE)
    # Remap absolute source directory to a relative path
    add_compile_options(-fdebug-prefix-map="${PROJECT_SOURCE_DIR_ABS}"=.)
    # Also remap build directory
    get_filename_component(PROJECT_BINARY_DIR_ABS "${CMAKE_BINARY_DIR}" ABSOLUTE)
    add_compile_options(-fdebug-prefix-map="${PROJECT_BINARY_DIR_ABS}"=.)
endif()

add_subdirectory(palette/pybind11)

# Include sub-projects.
add_subdirectory ("palette")

if (${BUILD_EXAMPLES})
    add_subdirectory ("standalone")
endif ()
