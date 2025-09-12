cmake_minimum_required(VERSION 3.21)

project(standalone)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Locate Qt.
find_package(Qt6Widgets CONFIG REQUIRED)

add_executable(standalone standalone.cpp)
target_link_libraries(standalone Qt6::Widgets palette)
