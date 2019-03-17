cmake_minimum_required(VERSION 3.1)

project(benchmark)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# Locate Qt.
find_package(Qt5Widgets CONFIG REQUIRED)

set(PYTHON_EXECUTABLE       ""    CACHE PATH "Python executable (x64 if >= 7.0)")
set(IDA_SKIP_ADD_LIBRARY ON)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark Qt5::Widgets palette)

# TODO: Add tests and install targets if needed.
