#pragma warning(push)

#ifdef _WIN32
#include <windows.h>
#endif

#if _MSC_VER
// Visual Studio
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#if __clang__
// clang
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wvarargs"
#pragma clang diagnostic ignored "-Wlogical-op-parentheses"
#endif

// clang header uses the functions
#define USE_STANDARD_FILE_FUNCTIONS
#define USE_DANGEROUS_FUNCTIONS

#define __DEFINE_PLUGIN_RETURN_CODES__

#include <cstdint>
#include <pro.h>
#include <ida.hpp>
#include <idp.hpp>
#include <diskio.hpp>
#include <kernwin.hpp>
#include <loader.hpp>
#include <moves.hpp>
#include <name.hpp>
#include <registry.hpp>
#include <typeinf.hpp>

#if IDA_SDK_VERSION >= 900
#define SHOULD_USE_TYPEINF 1
#else
#define SHOULD_USE_TYPEINF 0
#endif

#if IDA_SDK_VERSION < 900
#define get_ordinal_count get_ordinal_qty
#include <enum.hpp>
#include <struct.hpp>
#endif

#pragma warning(pop)
