IDA plugin CMake build-script
=============================

This repository holds CMake build scripts and a Python helper allowing 
compilation of C++ IDA plugins for Windows, macOS and Linux without
much user effort.

## Simple plugin example usage:

##### Create plugin repo
```bash
git init myplugin
cd myplugin
git submodule add https://github.com/zyantific/ida-cmake.git ida-cmake
mkdir src
touch src/myplugin.cpp CMakeLists.txt
```

##### CMakeLists.txt
```CMake
cmake_minimum_required(VERSION 3.1)
project(myplugin)

include("ida-cmake/cmake/IDA.cmake")

set(sources "src/myplugin.cpp")
add_ida_plugin(${CMAKE_PROJECT_NAME} ${sources})
```

##### src/myplugin.cpp
```cpp
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>

/**
 * @brief   Initialization callback for IDA.
 * @return  A @c PLUGIN_ constant from loader.hpp.
 */
int idaapi init()
{
    msg("%s", "Hello, IDA plugin world!\n");
    return PLUGIN_KEEP;
}

/**
 * @brief   Run callback for IDA.
 */
void idaapi run(int /*arg*/) {}

/**
 * @brief   Shutdown callback for IDA.
 */
void idaapi term() {}

plugin_t PLUGIN =
{
    IDP_INTERFACE_VERSION,
    0,
    &init,
    &term,
    &run,
    "My plugin name",
    "My plugin description",
    "My plugin menu entry text",
    nullptr, // plugin hotkey, e.g. "Ctrl-Shift-A"
};
```

##### Building and installing the plugin for IDA 6.95 on macOS
```bash
ida-cmake/build.py -i <ida-sdk-path> -t 6.95 \
    --idaq-path '/Applications/IDA Pro 6.95.app/Contents/MacOS/'
```
Substitute `<ida-sdks-path>` with a directory of the IDA SDK corresponding to your IDA version.
