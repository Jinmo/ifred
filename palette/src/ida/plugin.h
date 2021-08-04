#pragma warning(push)

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

#include <ida.hpp>
#include <idp.hpp>
#include <diskio.hpp>
#include <enum.hpp>
#include <kernwin.hpp>
#include <loader.hpp>
#include <moves.hpp>
#include <name.hpp>
#include <registry.hpp>
#include <struct.hpp>
#include <typeinf.hpp>

#pragma warning(pop)
