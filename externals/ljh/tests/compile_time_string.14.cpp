
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/compile_time_string.hpp"
#include <cstdint>

TEST_CASE("compile time string - hash", "[test_14][compile_time_string]" ) {
#if UINTPTR_MAX == 0xFFFFFFFF
	CHECK(ljh::compile_time_string_literal("ljh"   ).hash() == 0xB888C63);
	CHECK(ljh::compile_time_string_literal("link1j").hash() == 0xB76552E);
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
	CHECK(ljh::compile_time_string_literal("ljh"   ).hash() == 0x0000B888C63);
	CHECK(ljh::compile_time_string_literal("link1j").hash() == 0x6530B76552E);
#endif
}

TEST_CASE("compile time string - equals", "[test_14][compile_time_string]" ) {
	auto one = ljh::compile_time_string_literal("ljh");
	CHECK(one == ljh::compile_time_string_literal("ljh"));
	CHECK(one == "ljh");
}

TEST_CASE("compile time string - adding", "[test_14][compile_time_string]" ) {
	auto one = ljh::compile_time_string_literal("ljh"   );
	auto two = ljh::compile_time_string_literal("link1j");
	CHECK(one + two == ljh::compile_time_string_literal("ljhlink1j"));
	CHECK(one + "link1j" == ljh::compile_time_string_literal("ljhlink1j"));
}