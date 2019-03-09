cmake_minimum_required(VERSION 3.1)

project(benchmark)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ida_qt_libs "Gui;Core;Widgets")

# Locate Qt.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

# On unixes, we link against the Qt libs that ship with IDA.
# On Windows with IDA versions >= 7.0, link against .libs in IDA SDK.
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR
    (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND NOT ${IDA_VERSION} LESS 700))
        
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(ida_qt_glob_path "${IDA_INSTALL_DIR}/../Frameworks/Qt@QTLIB@")
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(ida_qt_glob_path "${IDA_INSTALL_DIR}/libQt5@QTLIB@.so*")
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(ida_qt_glob_path "${IDA_SDK}/lib/x64_win_qt/Qt5@QTLIB@.lib")
    endif ()

    foreach(cur_lib ${ida_qt_libs})
        string(REPLACE "@QTLIB@" ${cur_lib} cur_glob_path ${ida_qt_glob_path})
        file(GLOB_RECURSE qtlibpaths ${cur_glob_path})
        # On some platforms, we will find more than one libfile here. 
        # Either one is fine, just pick the first.
        foreach(p ${qtlibpaths})
            set(IDA_Qt${cur_lib}_LIBRARY ${p} CACHE FILEPATH "Path to IDA's Qt${cur_lib}")
            break()
        endforeach()
    endforeach()

    # On Windows, we hack Qt's "IMPLIB"s, on unix the .so location.
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(lib_property "IMPORTED_IMPLIB_RELEASE")
    else ()
        set(lib_property "IMPORTED_LOCATION_RELEASE")
    endif ()

    foreach (cur_lib ${ida_qt_libs})
        set_target_properties(
            "Qt5::${cur_lib}"
            PROPERTIES 
            ${lib_property} "${IDA_Qt${cur_lib}_LIBRARY}")
    endforeach()
endif ()

set(PYTHON_EXECUTABLE       ""    CACHE PATH "Python executable (x64 if >= 7.0)")
set(IDA_SKIP_ADD_LIBRARY ON)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark Qt5::Widgets Qt5::Core Qt5::Gui palette)

target_include_directories(benchmark
	PRIVATE C:/Users/berry/source/repos/palette/palette/src/)

# TODO: Add tests and install targets if needed.
