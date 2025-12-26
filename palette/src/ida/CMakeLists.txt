include(${IDASDK}/ida-cmake/idasdk.cmake)

set(PLUGIN_NAME          ida_palette)
set(PLUGIN_SOURCES       plugin.cpp python.cpp)
set(PLUGIN_LINK_LIBRARIES palette Qt6::Core Qt6::Gui Qt6::Widgets)

generate()

target_compile_definitions(ida_palette PRIVATE "QT_NAMESPACE=QT")

if (${PYTHON_SUPPORT})
    target_compile_definitions(ida_palette PRIVATE "PYTHON_SUPPORT=1")
else ()
    target_compile_definitions(ida_palette PRIVATE "PYTHON_SUPPORT=0")
endif ()

# In OSX, make ifred use @executable_path/python[64].dylib to use IDAPython bindings
if ((${PYTHON_SUPPORT}) AND (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin"))
    if (EA64)
        set(IDAPYTHON "${IDABIN}/plugins/idapython${PYTHON_VERSION_MAJOR}_64.dylib")
    else ()
        set(IDAPYTHON "${IDABIN}/plugins/idapython${PYTHON_VERSION_MAJOR}.dylib")
    endif ()
    target_link_libraries(ida_palette ${IDAPYTHON})
endif ()

# Link palette and python libraries
target_link_libraries(ida_palette ${python_libraries})

# On macOS with IDA, link against IDA's Qt frameworks
if(APPLE AND DEFINED IDABIN)
    get_filename_component(IDA_APP_DIR "${IDABIN}" DIRECTORY)
    set(QT_FRAMEWORK_PATH "${IDA_APP_DIR}/Frameworks")
    if(EXISTS "${QT_FRAMEWORK_PATH}/QtCore.framework")
        get_filename_component(QT_FRAMEWORK_PATH "${QT_FRAMEWORK_PATH}" ABSOLUTE)
        target_link_options(ida_palette PRIVATE "-F${QT_FRAMEWORK_PATH}")
        target_link_libraries(ida_palette
            "-framework QtCore"
            "-framework QtGui"
            "-framework QtWidgets"
        )
    endif()
endif()

if(WIN32)
    target_link_options(ida_palette PRIVATE "$<$<CONFIG:Release>:/DEBUG:NONE>" "/NOCOFFGRPINFO")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(APPLE)
        if(CMAKE_OSX_ARCHITECTURES)
            set(MAC_ARCH "${CMAKE_OSX_ARCHITECTURES}")
        else()
            set(MAC_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        if(MAC_ARCH STREQUAL "x86_64")
            add_custom_command(TARGET ida_palette POST_BUILD
                COMMENT "Stripping ${MAC_ARCH} binary"
                COMMAND strip -x $<TARGET_FILE:ida_palette>
            )
        endif()
    elseif(UNIX)
        add_custom_command(TARGET ida_palette POST_BUILD
            COMMENT "Stripping binary"
            COMMAND strip -s $<TARGET_FILE:ida_palette>
        )
    endif()
endif()
