
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// compile_time_string.hpp - v1.3
// SPDX-License-Identifier: BSL-1.0
//
// Requires C++14
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
//     1.3 Lowwer C++ requirment, add overloaded operators

#pragma once
#include "cpp_version.hpp"

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
		
		[[nodiscard]] constexpr compile_time_string() noexcept
		{
		}

		[[nodiscard]] constexpr compile_time_string(const char_type (&input)[Size + 1]) noexcept
		{
			LJH_IF_CONSTEXPR (Size != 0)
			{
				for (std::size_t i{0}; i < Size + 1; ++i)
				{
					content[i] = input[i];
				}
			}
		}
		
		[[nodiscard]] constexpr compile_time_string(const compile_time_string& other) noexcept
		{
			for (std::size_t i{0}; i < Size + 1; ++i)
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
		
		[[nodiscard]] constexpr char_type& operator[](std::size_t i)
		{
			if (i < Size)
				return content[i];
			throw std::out_of_range("");
		}

		[[nodiscard]] constexpr char_type operator[](std::size_t i) const
		{
			return i < Size ? content[i] : throw std::out_of_range("");
		}

#if __cpp_lib_string_view >= 201606L
		[[nodiscard]] constexpr operator std::basic_string_view<char_type>() const noexcept
		{
			return std::basic_string_view<char_type>{content, Size};
		}
#endif

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

#if __cpp_deduction_guides >= 201703L
	template<typename Char, std::size_t Size>
	compile_time_string(const Char (&)[Size]) -> compile_time_string<Char, Size - 1>;
	template<typename Char, std::size_t Size>
	compile_time_string(const compile_time_string<Char, Size>&) -> compile_time_string<Char, Size>;
#endif

	template <typename Char, int Size>
	constexpr auto compile_time_string_literal(const Char (&lit)[Size]) -> compile_time_string<Char, Size - 1> 
	{
		return compile_time_string<Char, Size - 1>(lit);
	}

#if LJH_CPP_VERSION >= LJH_CPP20_VERSION && __cpp_nontype_template_args >= 201911L
	inline namespace compile_time_string_literals
	{
		template<compile_time_string text> [[nodiscard]] constexpr auto operator ""_cts () { return text       ; }
		template<compile_time_string text> [[nodiscard]] constexpr auto operator ""_hash() { return text.hash(); }
	}
#endif

	template<typename C, std::size_t S1, std::size_t S2>
	[[nodiscard]] constexpr compile_time_string<C, S1 + S2> operator+(const compile_time_string<C, S1>& s1, const compile_time_string<C, S2>& s2)
	{
		compile_time_string<C, S1 + S2> new_string;
		int a = 0;
		for (; a < S1; a++)
			new_string[a] = s1[a];
		for (; a < new_string.size(); a++)
			new_string[a] = s2[a - S1];
		return new_string;
	}

	template<typename C, std::size_t S1, std::size_t S2>
	[[nodiscard]] constexpr compile_time_string<C, S1 + S2 - 1> operator+(const compile_time_string<C, S1>& s1, const C (&s2)[S2])
	{
		compile_time_string<C, S1 + S2 - 1> new_string;
		int a = 0;
		for (; a < S1; a++)
			new_string[a] = s1[a];
		for (; a < new_string.size(); a++)
			new_string[a] = s2[a - S1];
		return new_string;
	}

	template<typename C, std::size_t S1, std::size_t S2>
	[[nodiscard]] constexpr std::enable_if_t<S1 != S2, bool> operator==(const compile_time_string<C, S1>& s1, const compile_time_string<C, S2>& s2)
	{
		return false;
	}

	template<typename C, std::size_t S1>
	[[nodiscard]] constexpr bool operator==(const compile_time_string<C, S1>& s1, const compile_time_string<C, S1>& s2)
	{
		for (int a = 0; a < S1; a++)
			if (s1[a] != s2[a])
				return false;
		return true;
	}

	template<typename C, std::size_t S1, std::size_t S2>
	[[nodiscard]] constexpr std::enable_if_t<S1 != S2 - 1, bool> operator==(const compile_time_string<C, S1>& s1, const C (&s2)[S2])
	{
		return false;
	}

	template<typename C, std::size_t S1, std::size_t S2>
	[[nodiscard]] constexpr std::enable_if_t<S1 == S2 - 1, bool> operator==(const compile_time_string<C, S1>& s1, const C (&s2)[S2])
	{
		for (int a = 0; a < S1; a++)
			if (s1[a] != s2[a])
				return false;
		return true;
	}
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
