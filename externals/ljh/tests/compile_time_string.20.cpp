
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/compile_time_string.hpp"

TEST_CASE("compile time string - hash", "[test_20][compile_time_string]" ) {
	if constexpr (sizeof(std::size_t) == 4)
	{
		CHECK(ljh::compile_time_string{"ljh"   }.hash() == 0xB888C63);
		CHECK(ljh::compile_time_string{"link1j"}.hash() == 0xB76552E);
		
	}
	else if constexpr (sizeof(std::size_t) == 8)
	{
		CHECK(ljh::compile_time_string{"ljh"   }.hash() == 0x0000B888C63);
		CHECK(ljh::compile_time_string{"link1j"}.hash() == 0x6530B76552E);
	}
}

#if LJH_CPP_VERSION >= LJH_CPP20_VERSION && __cpp_nontype_template_args >= 201911L
TEST_CASE("compile time string - hash user define literal", "[test_20][compile_time_string]" ) {
	using namespace ljh::compile_time_string_literals;
	if constexpr (sizeof(std::size_t) == 4)
	{
		CHECK("ljh"_hash    == 0xB888C63);
		CHECK("link1j"_hash == 0xB76552E);
		
	}
	else if constexpr (sizeof(std::size_t) == 8)
	{
		CHECK("ljh"_hash    == 0x0000B888C63);
		CHECK("link1j"_hash == 0x6530B76552E);
	}
}

TEST_CASE("compile time string - user define literal", "[test_20][compile_time_string]" ) {
	using namespace ljh::compile_time_string_literals;
	auto test = "test"_cts;
	REQUIRE(test == "test");
}
#endif