
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/string_utils.hpp"

using namespace std::literals;

TEST_CASE("ltrim","[test_11][string_util]")
{
	std::string message{"     AAAAA     AAAAA     "};
	ljh::ltrim(message);
	REQUIRE(message == "AAAAA     AAAAA     ");
}

TEST_CASE("rtrim","[test_11][string_util]")
{
	std::string message{"     AAAAA     AAAAA     "};
	ljh::rtrim(message);
	REQUIRE(message == "     AAAAA     AAAAA");
}

TEST_CASE("trim","[test_11][string_util]")
{
	std::string message{"     AAAAA     AAAAA     "};
	ljh::trim(message);
	REQUIRE(message == "AAAAA     AAAAA");
}

TEST_CASE("ltrim_copy","[test_11][string_util]")
{
	std::string message{"     AAAAA     AAAAA     "};
	std::string copy = ljh::ltrim_copy(message);
	REQUIRE(copy == "AAAAA     AAAAA     ");
	REQUIRE(copy != message);
}

TEST_CASE("rtrim_copy","[test_11][string_util]")
{
	std::string message{"     AAAAA     AAAAA     "};
	std::string copy = ljh::rtrim_copy(message);
	REQUIRE(copy == "     AAAAA     AAAAA");
	REQUIRE(copy != message);
}

TEST_CASE("trim_copy","[test_11][string_util]")
{
	std::string message{"     AAAAA     AAAAA     "};
	std::string copy = ljh::trim_copy(message);
	REQUIRE(copy == "AAAAA     AAAAA");
	REQUIRE(copy != message);
}