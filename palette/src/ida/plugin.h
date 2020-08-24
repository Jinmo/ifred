#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)

// clang header uses the functions
#define USE_STANDARD_FILE_FUNCTIONS
#define USE_DANGEROUS_FUNCTIONS

#define __DEFINE_PLUGIN_RETURN_CODES__

#include <ida.hpp>
#include <idp.hpp>

#pragma warning(pop)

#include <kernwin.hpp>
#include <loader.hpp>
#include <name.hpp>
#include <struct.hpp>
#include <diskio.hpp>
#include <registry.hpp>
#include <moves.hpp>
#include <typeinf.hpp>
