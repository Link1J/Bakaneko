
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>
#include "ljh/version.hpp"

using namespace std::literals;

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
}

struct EqualsVersionMatcher : Catch::Matchers::MatcherGenericBase {
	EqualsVersionMatcher(ljh::version const& range)
		: range(range)
	{}

	bool match(ljh::version const& other) const {
		return range == other;
	}

	std::string describe() const override {
		return "is same as " + (std::string)range;
	}

private:
	ljh::version const& range;
};

auto EqualsVersion(const ljh::version& range) -> EqualsVersionMatcher {
	return EqualsVersionMatcher{range};
}

TEST_CASE("version constructors - string_view", "[test_17][version]")
{
	CHECK_THAT(ljh::version{"1.2.3.4"sv}, EqualsVersion({1,2,3,4}));
	CHECK_THAT(ljh::version{"1.2.3"sv  }, EqualsVersion({1,2,3,0}));
	CHECK_THAT(ljh::version{"1.2"sv    }, EqualsVersion({1,2,0,0}));
	CHECK_THAT(ljh::version{"1"sv      }, EqualsVersion({1,0,0,0}));
	CHECK_THAT(ljh::version{"...4"sv   }, EqualsVersion({0,0,0,4}));
	CHECK_THAT(ljh::version{"..3"sv    }, EqualsVersion({0,0,3,0}));
	CHECK_THAT(ljh::version{".2"sv     }, EqualsVersion({0,2,0,0}));
}
TEST_CASE("version constructors - string", "[test_17][version]")
{
	CHECK_THAT(ljh::version{"1.2.3.4"s}, EqualsVersion({1,2,3,4}));
	CHECK_THAT(ljh::version{"1.2.3"s  }, EqualsVersion({1,2,3,0}));
	CHECK_THAT(ljh::version{"1.2"s    }, EqualsVersion({1,2,0,0}));
	CHECK_THAT(ljh::version{"1"s      }, EqualsVersion({1,0,0,0}));
}
TEST_CASE("version constructors - const char*", "[test_17][version]")
{
	CHECK_THAT(ljh::version{"1.2.3.4"}, EqualsVersion({1,2,3,4}));
	CHECK_THAT(ljh::version{"1.2.3"  }, EqualsVersion({1,2,3,0}));
	CHECK_THAT(ljh::version{"1.2"    }, EqualsVersion({1,2,0,0}));
	CHECK_THAT(ljh::version{"1"      }, EqualsVersion({1,0,0,0}));
}
TEST_CASE("version constructors - ljh::limit::max", "[test_17][version]")
{
	CHECK_THAT(ljh::version(ljh::limit::max        ), EqualsVersion({ljh::version::max_value,ljh::version::max_value,ljh::version::max_value,ljh::version::max_value}));
	CHECK_THAT(ljh::version(ljh::limit::max,1      ), EqualsVersion({                      1,ljh::version::max_value,ljh::version::max_value,ljh::version::max_value}));
	CHECK_THAT(ljh::version(ljh::limit::max,1,2    ), EqualsVersion({                      1,                      2,ljh::version::max_value,ljh::version::max_value}));
	CHECK_THAT(ljh::version(ljh::limit::max,1,2,3  ), EqualsVersion({                      1,                      2,                      3,ljh::version::max_value}));
	CHECK_THAT(ljh::version(ljh::limit::max,1,2,3,4), EqualsVersion({                      1,                      2,                      3,                      4}));
}
TEST_CASE("version constructors - ljh::limit::min", "[test_17][version]")
{
	CHECK_THAT(ljh::version(ljh::limit::min        ), EqualsVersion({ljh::version::min_value,ljh::version::min_value,ljh::version::min_value,ljh::version::min_value}));
	CHECK_THAT(ljh::version(ljh::limit::min,1      ), EqualsVersion({                      1,ljh::version::min_value,ljh::version::min_value,ljh::version::min_value}));
	CHECK_THAT(ljh::version(ljh::limit::min,1,2    ), EqualsVersion({                      1,                      2,ljh::version::min_value,ljh::version::min_value}));
	CHECK_THAT(ljh::version(ljh::limit::min,1,2,3  ), EqualsVersion({                      1,                      2,                      3,ljh::version::min_value}));
	CHECK_THAT(ljh::version(ljh::limit::min,1,2,3,4), EqualsVersion({                      1,                      2,                      3,                      4}));
}

TEST_CASE("version compare - greater then", "[test_17][version]")
{
	CHECK(ljh::version{1,2,3,5} > ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,4,4} > ljh::version{1,2,3,4});
	CHECK(ljh::version{1,3,3,4} > ljh::version{1,2,3,4});
	CHECK(ljh::version{2,2,3,4} > ljh::version{1,2,3,4});
}
TEST_CASE("version compare - less then", "[test_17][version]")
{
	CHECK(ljh::version{1,2,3,3} < ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,2,4} < ljh::version{1,2,3,4});
	CHECK(ljh::version{1,1,3,4} < ljh::version{1,2,3,4});
	CHECK(ljh::version{0,2,3,4} < ljh::version{1,2,3,4});
}
TEST_CASE("version compare - equal to", "[test_17][version]")
{
	CHECK(ljh::version{1,2,3,4} == ljh::version{1,2,3,4});
}
TEST_CASE("version compare - not equal to", "[test_17][version]")
{
	CHECK(ljh::version{1,2,3,3} != ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,2,4} != ljh::version{1,2,3,4});
	CHECK(ljh::version{1,1,3,4} != ljh::version{1,2,3,4});
	CHECK(ljh::version{0,2,3,4} != ljh::version{1,2,3,4});
}
TEST_CASE("version compare - greater then equal to", "[test_17][version]")
{
	CHECK(ljh::version{1,2,3,4} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,3,5} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,4,4} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,3,3,4} >= ljh::version{1,2,3,4});
	CHECK(ljh::version{2,2,3,4} >= ljh::version{1,2,3,4});
}
TEST_CASE("version compare - less then equal to", "[test_17][version]")
{
	CHECK(ljh::version{1,2,3,4} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,3,3} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,2,2,4} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{1,1,3,4} <= ljh::version{1,2,3,4});
	CHECK(ljh::version{0,2,3,4} <= ljh::version{1,2,3,4});
}
