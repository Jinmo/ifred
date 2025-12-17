include("../../ida-cmake/cmake/IDA.cmake")

set(sources plugin.cpp python.cpp)

add_ida_plugin(ida_palette ${sources})

if(APPLE AND IDA_INSTALL_DIR)
    target_compile_definitions(ida_palette PUBLIC "QT_NAMESPACE=QT")
endif()

if (${PYTHON_SUPPORT})
    target_compile_definitions(ida_palette PRIVATE "PYTHON_SUPPORT=1")
else ()
    target_compile_definitions(ida_palette PRIVATE "PYTHON_SUPPORT=0")
endif ()

# In OSX, make ifred use @executable_path/python[64].dylib to use IDAPython bindings
# I'm not sure if it's a best way.
if ((${PYTHON_SUPPORT}) AND (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin"))
    if (IDA_EA_64)
        set(IDAPYTHON "${IDA_INSTALL_DIR}/plugins/idapython${PYTHON_VERSION_MAJOR}_64.dylib")
    else ()
        set(IDAPYTHON "${IDA_INSTALL_DIR}/plugins/idapython${PYTHON_VERSION_MAJOR}.dylib")
    endif ()
    target_link_libraries(ida_palette PRIVATE ${IDAPYTHON})
endif ()

# Link palette and python libraries
target_link_libraries(ida_palette PRIVATE palette ${python_libraries})
# On macOS with IDA, use Qt6 for headers (but don't link - we link IDA's Qt)
if(APPLE AND IDA_INSTALL_DIR)
    target_link_libraries(ida_palette PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
    set_target_properties(ida_palette PROPERTIES INTERFACE_LINK_LIBRARIES "")
else ()
    target_link_libraries(ida_palette
            PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets palette ${python_libraries})
endif()

# On macOS with IDA, link against IDA's Qt frameworks
if(APPLE AND IDA_INSTALL_DIR)
    get_filename_component(IDA_APP_DIR "${IDA_INSTALL_DIR}" DIRECTORY)
    set(QT_FRAMEWORK_PATH "${IDA_APP_DIR}/Frameworks")
    if(EXISTS "${QT_FRAMEWORK_PATH}/QtCore.framework")
        get_filename_component(QT_FRAMEWORK_PATH "${QT_FRAMEWORK_PATH}" ABSOLUTE)
        # Set framework search path for IDA's Qt
        target_link_options(ida_palette PRIVATE "-F${QT_FRAMEWORK_PATH}")
        # Link IDA's Qt frameworks
        target_link_libraries(ida_palette PRIVATE
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
            add_custom_command(TARGET ida_palette POST_BUILD
                COMMENT "1 ${MAC_ARCH}"
            )
        else()
            set(MAC_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
            add_custom_command(TARGET ida_palette POST_BUILD
                COMMENT "2 ${MAC_ARCH}"
            )
        endif()
        if(MAC_ARCH STREQUAL "x86_64")
            add_custom_command(TARGET ida_palette POST_BUILD
                COMMENT "Stripping ${MAC_ARCH} binary"
                COMMAND strip -x $<TARGET_FILE:ida_palette>
            )
        elseif(MAC_ARCH STREQUAL "arm64")
            add_custom_command(TARGET ida_palette POST_BUILD
                COMMENT "TODO: Find proper way to strip ${MAC_ARCH} binary"
                #COMMAND strip -x $<TARGET_FILE:ida_palette>
            )
        endif()
    elseif(UNIX)
        add_custom_command(TARGET ida_palette POST_BUILD
            COMMENT "Stripping binary"
            COMMAND strip -s $<TARGET_FILE:ida_palette>
        )
    endif()
endif()
