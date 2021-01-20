
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// compile_time_string.hpp - v1.2
// SPDX-License-Identifier: BSL-1.0
//
// Requires C++20
// Requires MVSC 19.26 or GCC 9
//
// ABOUT
//     Adds compile time strings
//
// USAGE
//    Meant for use in templates
//
// Version History
//     1.0 Inital Version
//     1.2 Null terminate compile_time_string, add User defined literals,
//         and change the hash function to support all char types

#pragma once
#include "cpp_version.hpp"
#if LJH_CPP_VERSION < LJH_CPP17_VERSION || __cpp_nontype_template_args < 201911L
#error "C++20 support for non-type template args is needed"
#endif

#include <string_view>
#include <stdexcept>
#include <cstddef>

namespace ljh
{
	template <typename Char, std::size_t Size>
	struct compile_time_string
	{
		using char_type = Char;

		char_type content[Size + 1] = {};

		[[nodiscard]] constexpr compile_time_string(const char_type (&input)[Size + 1]) noexcept
		{
			if constexpr (Size != 0)
			{
				for (std::size_t i{0}; i < Size + 1; ++i)
				{
					content[i] = input[i];
				}
			}
		}
		
		[[nodiscard]] constexpr compile_time_string(const compile_time_string<Char, Size> &other) noexcept
		{
			for (std::size_t i{0}; i < Size; ++i)
			{
				content[i] = other.content[i];
			}
		}

		[[nodiscard]] constexpr std::size_t size() const noexcept
		{
			return Size;
		}

		[[nodiscard]] constexpr const char_type* data() const noexcept
		{
			return content;
		}

		[[nodiscard]] constexpr const char_type* begin() const noexcept
		{
			return content;
		}

		[[nodiscard]] constexpr const char_type* end() const noexcept
		{
			return content + Size;
		}

		[[nodiscard]] constexpr char_type operator[](std::size_t i) const noexcept
		{
			return i < Size ? content[i] : throw std::out_of_range("");
		}

		[[nodiscard]] constexpr operator std::basic_string_view<char_type>() const noexcept
		{
			return std::basic_string_view<char_type>{content, Size};
		}

		// djb2 hash function
		[[nodiscard]] constexpr std::size_t hash() const noexcept
		{
			std::size_t hash = 5381;
			for (auto& c : *this)
				// hash * 33 + c
				hash = ((hash << 5) + hash) + c;
			return hash;
		}
	};

	template<typename Char, std::size_t Size>
	compile_time_string(const Char (&)[Size]) -> compile_time_string<Char, Size - 1>;
	template<typename Char, std::size_t Size>
	compile_time_string(const compile_time_string<Char, Size>& ) -> compile_time_string<Char, Size>;

	//inline namespace compile_time_string_literals
	//{
	//	template<ljh::compile_time_string text> [[nodiscard]] constexpr auto operator ""_cts () { return text       ; }
	//	template<ljh::compile_time_string text> [[nodiscard]] constexpr auto operator ""_hash() { return text.hash(); }
	//}
}

namespace std
{
	template<typename Char, std::size_t Size>
	struct hash<ljh::compile_time_string<Char, Size>>
	{
		std::size_t operator()(ljh::compile_time_string<Char, Size> const& s) const noexcept
		{
			return s.hash();
		}
	};
}
