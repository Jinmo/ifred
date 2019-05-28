cmake_minimum_required(VERSION 3.1)

include("ida-cmake/cmake/QtIDA.cmake")
include("ida-cmake/cmake/IDA.cmake")

# Some required settings
set(PYTHON_EXECUTABLE "" CACHE PATH "Python executable (x64 if >= 7.0)")

include(GenerateExportHeader)

set(sources src/widgets/palette_filter.cpp
            src/widgets/palette.cpp
            src/widgets/items.cpp
            src/widgets/item.cpp
            src/bindings/pypalette.cpp
            src/utils.cpp
            src/action.cpp
            src/palette_api.cpp
            res/theme_bundle.qrc
            include/widgets/palette_filter.h
            include/widgets/palette.h
            include/action.h
)

add_library(palette STATIC ${sources})
target_link_libraries(palette PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets pybind11::module)
target_compile_definitions(palette PUBLIC "QT_NAMESPACE=QT")

target_include_directories(palette
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_BINARY_DIR}/palette
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

GENERATE_EXPORT_HEADER(palette)

# IDA plugins

set(sources src/ida/plugin.cpp)

add_ida_qt_plugin(ida_palette ${sources})

# In OSX, make ifred use @executable_path/python[64].dylib to use IDAPython bindings
# I'm not sure if it's a best way.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	if(IDA_EA_64)
	    set(IDAPYTHON "${IDA_INSTALL_DIR}/plugins/python64.dylib")
	else()
	    set(IDAPYTHON "${IDA_INSTALL_DIR}/plugins/python.dylib")
	endif()
	target_link_libraries(ida_palette PRIVATE ${IDAPYTHON})
endif()

target_link_libraries(ida_palette
    PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets palette pybind11::module)
