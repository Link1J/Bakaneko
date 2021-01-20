
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/function_traits.hpp"

void LJH_CALLING_CONVENTION_vectorcall temp() noexcept {}

struct member
{
	int LJH_CALLING_CONVENTION_vectorcall function(int a, int b) const noexcept { return a + b; };
};

TEST_CASE("function_traits", "[function_traits]") {
	REQUIRE(ljh::function_traits<void() noexcept            >::is::function        );
	REQUIRE(ljh::function_traits<decltype(temp)             >::is::function        );
	REQUIRE(ljh::function_traits<decltype(&temp)            >::is::function_pointer);
	REQUIRE(ljh::function_traits<std::function<void()>      >::is::std_function    );
	REQUIRE(ljh::function_traits<decltype(&member::function)>::is::function_pointer);
}