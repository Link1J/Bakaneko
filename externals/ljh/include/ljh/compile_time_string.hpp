//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// compile_time_string.hpp - v1.1
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

#pragma once
#include "cpp_version.hpp"
#if LJH_CPP_VERSION < LJH_CPP17_VERSION || __cpp_nontype_template_args < 201911L
#error "C++20 support for non-type template args is needed"
#endif

#include <string_view>
#include <stdexcept>

namespace ljh
{
	template <typename Char, std::size_t Size, typename Traits = std::char_traits<Char>>
	struct compile_time_string
	{
		using char_type = Char;

		char_type content[Size] = {};

		constexpr compile_time_string(const char_type (&input)[Size + 1]) noexcept
		{
			if constexpr (Size != 0)
			{
				for (std::size_t i{0}; i < Size; ++i)
				{
					content[i] = input[i];
				}
			}
		}
		
		template <typename Traits2>
		constexpr compile_time_string(const compile_time_string<Char, Size, Traits2> &other) noexcept
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

		constexpr const char_type* begin() const noexcept
		{
			return content;
		}

		constexpr const char_type* end() const noexcept
		{
			return content + Size;
		}

		constexpr char_type operator[](std::size_t i) const noexcept
		{
			return i < Size ? content[i] : throw std::out_of_range("");
		}

		constexpr operator std::basic_string_view<char_type>() const noexcept
		{
			return std::basic_string_view<char_type>{content, Size};
		}

		static constexpr std::size_t FNV_offset_basis = 
			sizeof(std::size_t) == 4 ?         0x811C9DC5 :
			sizeof(std::size_t) == 8 ? 0xCBF29CE484222325 :
			0;
		static_assert(FNV_offset_basis != 0, "Unknown value for FNV_offset_basis with the current size of std::size_t");

		static constexpr std::size_t FNV_prime = 
			sizeof(std::size_t) == 4 ?         0x01000193 :
			sizeof(std::size_t) == 8 ? 0x00000100000001B3 :
			0;
		static_assert(FNV_prime != 0, "Unknown value for FNV_prime with the current size of std::size_t");

		constexpr std::enable_if_t<sizeof(char_type) == 1, std::size_t> hash() const noexcept
		{
			std::size_t hash_data = FNV_offset_basis;
			for (auto &byte_of_data : *this)
			{
				hash_data = hash_data ^ byte_of_data;
				hash_data = hash_data * FNV_prime;
			}
			return hash_data;
		}
	};

	template<typename Char, std::size_t Size, typename Traits = std::char_traits<Char>>
	compile_time_string(const Char (&)[Size]) -> compile_time_string<Char, Size - 1, Traits>;
	template<typename Char, std::size_t Size, typename Traits = std::char_traits<Char>>
	compile_time_string(compile_time_string<Char, Size>) -> compile_time_string<Char, Size, Traits>;
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
