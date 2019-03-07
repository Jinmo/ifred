#
# The MIT License (MIT)
#
# Copyright (c) 2017 Joel Höner <athre0z@zyantific.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0054 NEW)

# =============================================================================================== #
# Overridable options                                                                             #
# =============================================================================================== #

set(IDA_BINARY_64         OFF   CACHE BOOL "Build a 64 bit binary (IDA >= 7.0)"             )
set(IDA_EA_64             OFF   CACHE BOOL "Build for 64 bit IDA (ida64, sizeof(ea_t) == 8)")
set(IDA_SDK               ""    CACHE PATH "Path to IDA SDK"                                )
set(IDA_INSTALL_DIR       ""    CACHE PATH "Install path of IDA"                            )
set(IDA_VERSION           690   CACHE INT  "IDA Version to build for (e.g. 6.9 is 690)."    )
set(IDA_ENABLE_QT_SUPPORT OFF   CACHE BOOL "Enable support for building plugins with Qt"    )

# =============================================================================================== #
# General preparation                                                                             #
# =============================================================================================== #

if (IDA_VERSION LESS 690)
    message(FATAL_ERROR "IDA versions below 6.9 are no longer supported.")
endif ()

# We need to save our path here so we have it available in functions later on.
set(ida_cmakelist_path ${CMAKE_CURRENT_LIST_DIR})

# Library dependencies and include pathes
if (WIN32)
    if (IDA_EA_64)
        set(ida_lib_path_ea "64")
    else ()
        set(ida_lib_path_ea "32")
    endif ()

    if (IDA_BINARY_64)
        set(ida_lib_path_binarch "x64")
    else ()
        set(ida_lib_path_binarch "x64")
    endif ()

    # On Windows, we use HR's lib files shipped with the SDK.
    set(IDA_LIB_DIR "${IDA_SDK}/lib/${ida_lib_path_binarch}_win_vc_${ida_lib_path_ea}"
        CACHE PATH "IDA SDK library path" FORCE)

    message(STATUS "IDA library path: ${IDA_LIB_DIR}")

    if (NOT EXISTS ${IDA_LIB_DIR})
        set(IDA_LIB_DIR NOTFOUND)
    endif ()

    find_library(IDA_IDA_LIBRARY NAMES "ida" PATHS ${IDA_LIB_DIR} REQUIRED)
    list(APPEND ida_libraries ${IDA_IDA_LIBRARY})
    find_library(IDA_PRO_LIBRARY NAMES "pro" PATHS ${IDA_LIB_DIR})
    if (IDA_PRO_LIBRARY)
        list(APPEND ida_libraries ${IDA_PRO_LIBRARY})
    endif ()
elseif (UNIX)
    if (NOT IDA_BINARY_64)
        set(CMAKE_C_FLAGS   "-m32" CACHE STRING "C compiler flags"   FORCE)
        set(CMAKE_CXX_FLAGS "-m32" CACHE STRING "C++ compiler flags" FORCE)
    endif ()

    # On unixoid platforms, we link against IDA directly.
    if (IDA_EA_64)
        find_library(IDA_IDA_LIBRARY NAMES "ida64" PATHS ${IDA_INSTALL_DIR} REQUIRED)
    else ()
        find_library(IDA_IDA_LIBRARY NAMES "ida" PATHS ${IDA_INSTALL_DIR} REQUIRED)
    endif ()
    list(APPEND ida_libraries ${IDA_IDA_LIBRARY})
endif ()

set(ida_libraries ${ida_libraries} CACHE INTERNAL "IDA libraries" FORCE)

# =============================================================================================== #
# Qt support                                                                                      #
# =============================================================================================== #

if (IDA_ENABLE_QT_SUPPORT)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    set(ida_qt_libs "Gui;Core;Widgets")

    # Locate Qt.
    find_package(Qt5Widgets REQUIRED)

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
endif ()

# =============================================================================================== #
# Functions for adding IDA plugin targets                                                         #
# =============================================================================================== #

function (add_ida_plugin plugin_name)
    set(sources ${ARGV})
    if (sources)
        list(REMOVE_AT sources 0)
    endif ()

    # Define target
    string(STRIP "${sources}" sources)
    add_library(${plugin_name} SHARED ${sources})

    # Compiler specific properties.
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_definitions(${plugin_name} PUBLIC "__VC__")
        target_compile_options(${plugin_name} PUBLIC "/wd4996" "/MP")
    endif ()

    # General definitions required throughout all kind of IDA modules.
    target_compile_definitions(${plugin_name} PUBLIC
        "NO_OBSOLETE_FUNCS"
        "__IDP__")

    target_include_directories(${plugin_name} PUBLIC "${IDA_SDK}/include")

    if (IDA_BINARY_64)
        target_compile_definitions(${plugin_name} PUBLIC "__X64__")
    endif ()

    # OS specific stuff.
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_compile_definitions(${plugin_name} PUBLIC "__NT__")

        if (IDA_BINARY_64)
            if (IDA_EA_64)
                set(plugin_extension "64.dll")
            else ()
                set(plugin_extension ".dll")
            endif ()
        else ()
            if (IDA_EA_64)
                set(plugin_extension ".p64")
            else()
                set(plugin_extension ".plw")
            endif()
        endif ()
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        target_compile_definitions(${plugin_name} PUBLIC "__MAC__")

        if (IDA_BINARY_64)
            if (IDA_EA_64)
                set(plugin_extension "64.dylib")
            else ()
                set(plugin_extension ".dylib")
            endif ()
        else ()
            if (IDA_EA_64)
                set(plugin_extension ".pmc64")
            else()
                set(plugin_extension ".pmc")
            endif()
        endif ()
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        target_compile_definitions(${plugin_name} PUBLIC "__LINUX__")

        if (IDA_BINARY_64)
            if (IDA_EA_64)
                set(plugin_extension "64.so")
            else ()
                set(plugin_extension ".so")
            endif ()
        else ()
            if (IDA_EA_64)
                set(plugin_extension ".plx64")
            else()
                set(plugin_extension ".plx")
            endif()
        endif ()
    endif ()

    # Suppress "lib" prefix on Unix and alter the file extension.
    set_target_properties(${plugin_name} PROPERTIES
        PREFIX ""
        SUFFIX ${plugin_extension}
        OUTPUT_NAME ${plugin_name})

    if (IDA_EA_64)
        target_compile_definitions(${plugin_name} PUBLIC "__EA64__")
    endif ()

    # Link against IDA (or the SDKs libs on Windows).
    target_link_libraries(${plugin_name} ${ida_libraries})

    # Define install rule
    install(TARGETS ${plugin_name} DESTINATION plugins)

    # When generating for Visual Studio, 
    # generate user file for convenient debugging support.
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        if (IDA_VERSION LESS 700)
            if (IDA_EA_64)
                set(idaq_exe "idaq64.exe")
            else ()
                set(idaq_exe "idaq.exe")
            endif ()
        else ()
            if (IDA_EA_64)
                set(idaq_exe "ida64.exe")
            else ()
                set(idaq_exe "ida.exe")
            endif ()
        endif ()

        file(
            TO_NATIVE_PATH 
            "${IDA_INSTALL_DIR}/${idaq_exe}" 
            idaq_exe_native_path)
        configure_file(
            "${ida_cmakelist_path}/template.vcxproj.user" 
            "${plugin_name}.vcxproj.user" 
            @ONLY)
    endif ()
endfunction (add_ida_plugin)

# =============================================================================================== #
# Functions for adding IDA plugin targets with Qt support                                         #
# =============================================================================================== #

function (add_ida_qt_plugin plugin_name)
    set(sources ${ARGV})
    if (sources)
        list(REMOVE_AT sources 0)
    endif ()

    # Divide between UI and resource files and regular C/C++ sources. 
    foreach (cur_file ${sources})
        if (${cur_file} MATCHES ".*\\.ui")
            list(APPEND ui_sources ${cur_file})
        elseif (${cur_file} MATCHES ".*\\.qrc")
            list(APPEND rsrc_sources ${cur_file})
        else ()
            list(APPEND non_ui_sources ${cur_file})
        endif ()
    endforeach ()

    # Compile UI files.
    QT5_WRAP_UI(form_headers ${ui_sources})

    # Compile resources.
    QT5_ADD_RESOURCES(rsrc_headers ${rsrc_sources})

    # Add plugin.
    add_ida_plugin(${plugin_name} ${non_ui_sources} ${form_headers} ${rsrc_headers})
    target_compile_definitions(${plugin_name} PUBLIC "QT_NAMESPACE=QT")

    # Link against Qt.
    foreach (qtlib Core;Gui;Widgets)
        if (DEFINED IDA_QtCore_LIBRARY)
            set_target_properties(
                "Qt5::${qtlib}"
                PROPERTIES 
                IMPORTED_LOCATION_RELEASE "${IDA_Qt${qtlib}_LIBRARY}")
        endif ()
        target_link_libraries(${CMAKE_PROJECT_NAME} "Qt5::${qtlib}")
    endforeach()
endfunction ()
