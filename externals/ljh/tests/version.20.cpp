
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include "ljh/version.hpp"

#if __cpp_impl_three_way_comparison >= 201907L && __has_include(<compare>) && !defined(__apple_build_version__)
namespace Catch
{
	template<>
	struct StringMaker<ljh::version>
	{
		static std::string convert(ljh::version const& value)
		{
			return static_cast<std::string>(value);
		}
	};
	template<>
	struct StringMaker<std::strong_ordering>
	{
		static std::string convert(std::strong_ordering const& value)
		{
			if (value == std::strong_ordering::equal  ) return "std::strong_ordering::equal"  ;
			if (value == std::strong_ordering::greater) return "std::strong_ordering::greater";
			if (value == std::strong_ordering::less   ) return "std::strong_ordering::less"   ;
			return "";
		}
	};
}

TEST_CASE("version using three way operator - greater then", "[test_20][version]") 
{	
	CHECK(std::strong_ordering::greater == ljh::version{1,2,3,4} <=> ljh::version{1,2,3,3});
	CHECK(std::strong_ordering::greater == ljh::version{1,2,3,4} <=> ljh::version{1,2,2,4});
	CHECK(std::strong_ordering::greater == ljh::version{1,2,3,4} <=> ljh::version{1,1,3,4});
	CHECK(std::strong_ordering::greater == ljh::version{1,2,3,4} <=> ljh::version{0,2,3,4});

	CHECK(ljh::version{1,2,3,5} > ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,4,4} > ljh::version{1,2,3,4});
	CHECK(ljh::version{1,3,3,4} > ljh::version{1,2,3,4});
	CHECK(ljh::version{2,2,3,4} > ljh::version{1,2,3,4});
}
TEST_CASE("version using three way operator - less then", "[test_20][version]")
{
	CHECK(std::strong_ordering::less == ljh::version{1,2,3,4} <=> ljh::version{1,2,3,5});
	CHECK(std::strong_ordering::less == ljh::version{1,2,3,4} <=> ljh::version{1,2,4,4});
	CHECK(std::strong_ordering::less == ljh::version{1,2,3,4} <=> ljh::version{1,3,3,4});
	CHECK(std::strong_ordering::less == ljh::version{1,2,3,4} <=> ljh::version{2,2,3,4});

	CHECK(ljh::version{1,2,3,3} < ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,2,4} < ljh::version{1,2,3,4});
	CHECK(ljh::version{1,1,3,4} < ljh::version{1,2,3,4});
	CHECK(ljh::version{0,2,3,4} < ljh::version{1,2,3,4});
}

TEST_CASE("version using three way operator - equal to", "[test_20][version]")
{
	CHECK(std::strong_ordering::equal == ljh::version{1,2,3,4} <=> ljh::version{1,2,3,4});

	CHECK(ljh::version{1,2,3,4} == ljh::version{1,2,3,4});
}

TEST_CASE("version using three way operator - not equal to", "[test_20][version]")
{
	CHECK(std::strong_ordering::equal != ljh::version{1,2,3,4} <=> ljh::version{4,3,2,1});
	CHECK(std::strong_ordering::equal != ljh::version{1,2,3,4} <=> ljh::version{1,2,3,3});
	CHECK(std::strong_ordering::equal != ljh::version{1,2,3,4} <=> ljh::version{1,2,2,4});
	CHECK(std::strong_ordering::equal != ljh::version{1,2,3,4} <=> ljh::version{1,1,3,4});
	CHECK(std::strong_ordering::equal != ljh::version{1,2,3,4} <=> ljh::version{0,2,3,4});

	CHECK(ljh::version{1,2,3,3} != ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,2,4} != ljh::version{1,2,3,4});
	CHECK(ljh::version{1,1,3,4} != ljh::version{1,2,3,4});
	CHECK(ljh::version{0,2,3,4} != ljh::version{1,2,3,4});
}

TEST_CASE("version using three way operator - greater then equal to", "[test_20][version]")
{
	CHECK(ljh::version{1,2,3,4} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,3,5} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,4,4} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,3,3,4} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{2,2,3,4} >= ljh::version{1,2,3,4});
}

TEST_CASE("version using three way operator - less then equal to", "[test_20][version]")
{
	CHECK(ljh::version{1,2,3,4} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,3,3} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,2,4} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,1,3,4} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{0,2,3,4} <= ljh::version{1,2,3,4});
}

#endif