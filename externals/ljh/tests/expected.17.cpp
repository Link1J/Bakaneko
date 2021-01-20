
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/expected.hpp"
#include <type_traits>

struct trivial_data_type
{
	int a, b, c;
};
static_assert(std::is_trivial<trivial_data_type>::value);

struct not_move
{
	not_move() = default;
	~not_move() = default;
	not_move(const not_move&) = default;
	not_move(not_move&&) = delete;
	not_move& operator=(const not_move&) = default;
	not_move& operator=(not_move&&) = delete;
};

struct not_copy
{
	not_copy() = default;
	~not_copy() = default;
	not_copy(not_copy&&) = default;
	not_copy& operator=(not_copy&&) = default;
};

TEST_CASE("expected<trivial_data_type, int>::emplace", "[test_17][expected][emplace]")
{
	ljh::expected<trivial_data_type, int> test;
	REQUIRE(test.has_value());

	SECTION("emplace trivial_data_type")
	{
		test.emplace(1, 2, 3);
		REQUIRE(test.has_value());

		SECTION("Overwrite with error")
		{
			test = ljh::unexpected{100};
			REQUIRE(!test.has_value());
		}
	}
}

TEST_CASE("expected<void, int>", "[test_17][expected]")
{
	ljh::expected<void, int> test;
	REQUIRE(test.has_value());
}

TEST_CASE("expected<int, int>", "[test_17][expected]")
{
	ljh::expected<int, int> test;
	REQUIRE(test.has_value());
}

TEST_CASE("expected<not_move, int>", "[test_17][expected]")
{
	ljh::expected<not_move, int> test;
	REQUIRE(test.has_value());
	REQUIRE(std::is_copy_constructible<ljh::expected<not_move, int>>::value);
	REQUIRE(std::is_move_constructible<ljh::expected<not_move, int>>::value);
	REQUIRE(std::is_copy_assignable   <ljh::expected<not_move, int>>::value);
	REQUIRE(std::is_move_assignable   <ljh::expected<not_move, int>>::value);
}

TEST_CASE("expected<not_copy, int>", "[test_17][expected]")
{
	ljh::expected<not_copy, int> test;
	REQUIRE(test.has_value());
	REQUIRE(!std::is_copy_constructible<ljh::expected<not_copy, int>>::value);
	REQUIRE( std::is_move_constructible<ljh::expected<not_copy, int>>::value);
	REQUIRE(!std::is_copy_assignable   <ljh::expected<not_copy, int>>::value);
	REQUIRE( std::is_move_assignable   <ljh::expected<not_copy, int>>::value);
}

TEST_CASE("expected<int, int> ==", "[test_17][expected][equality]")
{
	ljh::expected<int, int> test_ed    ;
	ljh::expected<int, int> test_e1 = 1;
	ljh::expected<int, int> test_u1 = ljh::unexpected{1};

	REQUIRE(test_ed == ljh::expected<int, int>{});
	REQUIRE(test_e1 != ljh::expected<int, int>{});
	REQUIRE(test_u1 != test_e1                  );
}

TEST_CASE("unexpected<const char*>", "[test_17][expected][unexpected]")
{
	ljh::unexpected<const char*> test{"Test"};
	// is a compile test
}