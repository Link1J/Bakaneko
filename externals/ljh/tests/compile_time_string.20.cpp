
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>

#if __cpp_nontype_template_args >= 201911

#include "ljh/compile_time_string.hpp"

TEST_CASE("compile time string", "[test_20][compile_time_string]" ) {
	if constexpr (sizeof(std::size_t) == 4)
	{
		CHECK(ljh::compile_time_string{"ljh"   }.hash() == 0x564AB9C1);
		CHECK(ljh::compile_time_string{"link1j"}.hash() == 0x868C17C6);
		
	}
	else if constexpr (sizeof(std::size_t) == 8)
	{
		CHECK(ljh::compile_time_string{"ljh"   }.hash() == 0x12468A191DA726C1);
		CHECK(ljh::compile_time_string{"link1j"}.hash() == 0x54EFEFC5BB2C6526);
	}
}

#endif