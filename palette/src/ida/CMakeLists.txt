project(palette)

set(sources src/ida/plugin.cpp)

add_ida_qt_plugin(ida_palette ${sources})
target_link_libraries(ida_palette PUBLIC palette)
