
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/cpp_version.hpp"

// Currently GCC doesn't work, but GCC 10 does not define __cpp_nontype_template_args correctly. Maybe GCC 11 will work?
#if __cpp_nontype_template_args >= 201911L && (defined(LJH_COMPILER_GCC) || defined(LJH_COMPILER_MSVC) || (!defined(LJH_COMPILER_APPLE_CLANG) && LJH_CLANG_VERSION >= LJH_COMPILER_GEN_VERSION(12, 0)) || (defined(LJH_COMPILER_APPLE_CLANG) && LJH_CLANG_VERSION >= LJH_COMPILER_GEN_VERSION(14, 0)))
#include "ljh/delay_loaded_functions.hpp"

#if defined(LJH_TARGET_Windows)
#include <windows.h>
#endif

TEST_CASE("delay_loaded_functions", "[test_20][delay_loaded_functions]" ) {
#if defined(LJH_TARGET_Windows)
	ljh::delay_load::function<"kernel32.dll", "FreeLibrary", BOOL __stdcall(HMODULE)> function_test;
#elif defined(LJH_TARGET_Linux)
	ljh::delay_load::function<"libdl.so", "dlclose", int(void*)> function_test;
#elif defined(LJH_TARGET_MacOS)
	ljh::delay_load::function<"libdyld.dylib", "FreeLibrary", int(void*)> function_test;
#endif
	REQUIRE(function_test.is_loadable());
}
#endif