
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/system_info.hpp"

TEST_CASE("system_info::get_platform()", "[test_17][system_info]")
{
	auto test = ljh::system_info::get_platform();
	REQUIRE(test.has_value());
}

TEST_CASE("system_info::get_version()", "[test_17][system_info]")
{
	auto test = ljh::system_info::get_version();
	REQUIRE(test.has_value());
	REQUIRE(test.value() != "0");
}

TEST_CASE("system_info::get_string()", "[test_17][system_info]")
{
	auto test = ljh::system_info::get_string();
	REQUIRE(test.has_value());
	REQUIRE(!test.value().empty());
}

TEST_CASE("system_info::get_sdk()", "[test_17][system_info]")
{
	auto test = ljh::system_info::get_sdk();
#if defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	REQUIRE(!test.has_value());
	REQUIRE(test.error() == ljh::system_info::error::no_info);
#else
	REQUIRE(test.has_value());
#endif
}


#if !defined(LJH_TARGET_Windows)
TEST_CASE("system_info::get_model()", "[test_17][system_info]")
{
	auto test = ljh::system_info::get_model();
#if defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	REQUIRE(!test.has_value());
	REQUIRE(test.error() == ljh::system_info::error::no_info);
#else
	REQUIRE(test.has_value());
	REQUIRE(!test.value().empty());
#endif
}

TEST_CASE("system_info::get_manufacturer()", "[test_17][system_info]")
{
	auto test = ljh::system_info::get_manufacturer();
#if defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	REQUIRE(!test.has_value());
	REQUIRE(test.error() == ljh::system_info::error::no_info);
#else
	REQUIRE(test.has_value());
	REQUIRE(!test.value().empty());
#endif
}
#endif