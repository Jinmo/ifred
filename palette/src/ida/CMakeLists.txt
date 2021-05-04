include("../../ida-cmake/cmake/IDA.cmake")

set(sources plugin.cpp python.cpp)

add_ida_qt_plugin(ida_palette ${sources})
if (${PYTHON_SUPPORT})
    target_compile_definitions(ida_palette PRIVATE "PYTHON_SUPPORT=1")
else ()
    target_compile_definitions(ida_palette PRIVATE "PYTHON_SUPPORT=0")
endif ()

# In OSX, make ifred use @executable_path/python[64].dylib to use IDAPython bindings
# I'm not sure if it's a best way.
if ((${PYTHON_SUPPORT}) AND (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin"))
    if (IDA_EA_64)
        set(IDAPYTHON "${IDA_INSTALL_DIR}/plugins/python64.dylib")
    else ()
        set(IDAPYTHON "${IDA_INSTALL_DIR}/plugins/python.dylib")
    endif ()
    target_link_libraries(ida_palette PRIVATE ${IDAPYTHON})
endif ()

target_link_libraries(ida_palette
        PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets palette ${python_libraries})
