
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// version.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// Requires type_traits.hpp and char_convertions.hpp and cpp_version.hpp
// 
// ABOUT
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#include "type_traits.hpp"
#include "char_convertions.hpp"
#include "cpp_version.hpp"

#include <limits>
#include <string>
#include <string_view>
#if LJH_CPP_VERSION > LJH_CPP17_VERSION
#    if __has_include(<compare>)
#        include <compare>
#    endif
#endif

#undef min
#undef max

namespace ljh
{
	namespace limit
	{
		struct limiting {};
		struct max_t    : limiting {}; inline constexpr max_t    max   {};
		struct min_t    : limiting {}; inline constexpr min_t    min   {};
		struct equal_t  : limiting {}; inline constexpr equal_t  equal {};
		struct always_t : limiting {}; inline constexpr always_t always{};
		struct never_t  : limiting {}; inline constexpr never_t  never {};
	}

	struct version
	{
		using value_type = unsigned int;
		static constexpr value_type min_value = std::numeric_limits<value_type>::min();
		static constexpr value_type max_value = std::numeric_limits<value_type>::max();

		template<class _char, class _traits = std::char_traits<_char>>
		version(std::basic_string_view<_char, _traits> text)
		{
			constexpr auto npos = std::basic_string_view<_char, _traits>::npos;

			auto dot_pos = text.find('.');
			if (dot_pos == npos) { from_string(_major, text); return; }
			auto sub = text.substr(0, dot_pos);
			from_string(_major, sub);
			text = text.substr(dot_pos + 1);

			dot_pos = text.find('.');
			if (dot_pos == npos) { from_string(_minor, text); return; }
			sub = text.substr(0, dot_pos);
			from_string(_minor, sub);
			text = text.substr(dot_pos + 1);

			dot_pos = text.find('.');
			if (dot_pos == npos) { from_string(_build, text); return; }
			sub = text.substr(0, dot_pos);
			from_string(_build, sub);
			text = text.substr(dot_pos + 1);

			dot_pos = text.find('.');
			if (dot_pos == npos) { from_string(_revision, text); return; }
			sub = text.substr(0, dot_pos);
			from_string(_revision, sub);
			text = text.substr(dot_pos + 1);
		}

		template<class _char, class _traits = std::char_traits<_char>, class _alloc = std::allocator<_char>>
		version(std::basic_string<_char, _traits, _alloc> text)
			: version(std::basic_string_view<_char, _traits>{text.data(),text.size()})
		{}

		template<typename _char, typename = typename std::enable_if_t<is_char_type_v<_char>>>
		version(_char const * text)
			: version(std::basic_string_view{text})
		{}

		version(value_type major = 0, value_type minor = 0, value_type build = 0, value_type revision = 0)
			: _major(major), _minor(minor), _build(build), _revision(revision)
		{
		}

		version(limit::max_t, value_type major = max_value, value_type minor = max_value, value_type build = max_value, value_type revision = max_value)
			: _major(major), _minor(minor), _build(build), _revision(revision)
		{
		}

		version(limit::min_t, value_type major = min_value, value_type minor = min_value, value_type build = min_value, value_type revision = min_value)
			: _major(major), _minor(minor), _build(build), _revision(revision)
		{
		}

// Apple Clang is dumb, so don't use <=> with it
#if __cpp_impl_three_way_comparison >= 201907L && __has_include(<compare>) && !defined(__apple_build_version__)
		std::strong_ordering operator<=>(const version& rhs) const 
		{
			if (auto cmp = _major <=> rhs._major; cmp != std::strong_ordering::equal) { return cmp; }
			if (auto cmp = _minor <=> rhs._minor; cmp != std::strong_ordering::equal) { return cmp; }
			if (auto cmp = _build <=> rhs._build; cmp != std::strong_ordering::equal) { return cmp; }
			return _revision <=> rhs._revision;
		}
		bool operator==(const version&) const = default;
#else
		bool operator==(const version& rhs) const
		{
			if (_major != rhs._major) { return false; }
			if (_minor != rhs._minor) { return false; }
			if (_build != rhs._build) { return false; }
			return _revision == rhs._revision;
		}
		bool operator<(const version& rhs) const
		{
			if (_major < rhs._major) { return true; }
			if (_minor < rhs._minor) { return true; }
			if (_build < rhs._build) { return true; }
			return _revision < rhs._revision;
		}
		bool operator!=(const version& rhs) const
		{
			return !(*this == rhs);
		}
		bool operator>(const version& rhs) const
		{
			return rhs < *this;
		}
		bool operator>=(const version& rhs) const
		{
			return !(*this < rhs);
		}
		bool operator<=(const version& rhs) const
		{
			return !(rhs < *this);
		}
#endif

		auto major   () const { return _major   ; };
		auto minor   () const { return _minor   ; };
		auto build   () const { return _build   ; };
		auto revision() const { return _revision; };

		operator std::string() const
		{
			return std::to_string(major())+'.'+std::to_string(minor())+'.'+std::to_string(build())+'.'+std::to_string(revision());
		}

	private:
		value_type _major    = 0;
		value_type _minor    = 0;
		value_type _build    = 0;
		value_type _revision = 0;
	};
}

template<typename T>
std::basic_ostream<T, std::char_traits<T>>& operator<<(std::basic_ostream<T, std::char_traits<T>>& os, const ljh::version& version)
{
	os << static_cast<std::string>(version);
	return os;
}