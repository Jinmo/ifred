cmake_minimum_required(VERSION 3.1)

include("ida-cmake/cmake/QtIDA.cmake")
# find_package(Qt5Widgets CONFIG REQUIRED)

include("ida-cmake/cmake/IDA.cmake")

# Some required settings
set(PYTHON_EXECUTABLE "" CACHE PATH "Python executable (x64 if >= 7.0)")

include(GenerateExportHeader)

set(sources src/widgets/myfilter.cpp
			src/widgets/qsearch.cpp
			src/utils.cpp
			src/widgets/qpalette_inner.cpp
			src/widgets/qpalettecontainer.cpp
			src/widgets/qitems.cpp
			src/widgets/qitem.cpp
			src/widgets/palette_manager.cpp
			src/bindings/pypalette.cpp
			res/theme_bundle.qrc
            include/widgets/myfilter.h
            include/widgets/qpalettecontainer.h
            include/widgets/qpalette_inner.h
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
target_link_libraries(ida_palette
	PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets palette pybind11::module)
