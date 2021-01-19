
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/system_directories.hpp"

TEST_CASE("system_directories::home()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::home().empty());
}

TEST_CASE("system_directories::cache()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::cache().empty());
}

TEST_CASE("system_directories::config()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::config().empty());
}

TEST_CASE("system_directories::data()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::data().empty());
}

TEST_CASE("system_directories::documents()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::documents().empty());
}

TEST_CASE("system_directories::desktop()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::desktop().empty());
}

TEST_CASE("system_directories::pictures()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::pictures().empty());
}

TEST_CASE("system_directories::music()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::music().empty());
}

TEST_CASE("system_directories::videos()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::videos().empty());
}

TEST_CASE("system_directories::downloads()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::downloads().empty());
}

TEST_CASE("system_directories::save_games()", "[test_98][system_directories]")
{
	REQUIRE(!ljh::system_directories::save_games().empty());
}
