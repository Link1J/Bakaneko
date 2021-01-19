
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// char_convertions.hpp - v1.1
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++14
// 
// ABOUT
//     Allows convertions from strings to basic types
//
// USAGE
//
// Version History
//     1.0 Inital Version
//     1.1 Fix build error on linux

#pragma once

#include "cpp_version.hpp"
#include "type_traits.hpp"
#include <type_traits>
#include <string>
#include <cmath>
#if LJH_CPP_VERSION >= LJH_CPP17_VERSION
#include <string_view>
#endif

namespace _ljh
{
	template<typename _conv, typename _char>
	void from_string(_conv& value, _char* text, size_t size)
	{
		if (size == 0) { return; }

		int a = 0;

		LJH_IF_CONSTEXPR (std::is_signed<_conv>::value)
		{
			if (text[0] == '-') { a = 1; }
		}
		
		for (; a < (int)size; a++)
		{
			if (!(text[a] >= '0' && text[a] <= '9'))
			{
				break;
			}
		}
		a--;

		for (auto max = a; a > -1; a--)
		{
			LJH_IF_CONSTEXPR (std::is_signed<_conv>::value)
			{
				if (text[a] == '-')
				{
					value = -value;
					continue;
				}
			}
			value += static_cast<_conv>((text[a] - '0') * pow(10,max-a));
		}
	}
}

namespace ljh
{
	template<typename _conv, class _char, class _traits = std::char_traits<_char>>
	typename std::enable_if<std::is_integral<_conv>::value>::type from_string(_conv& value, std::basic_string_view<_char, _traits> text)
	{
		_ljh::from_string(value, text.data(), text.size());
	}

	template<typename _conv, class _char, class _traits = std::char_traits<_char>, class _alloc = std::allocator<_char>>
	typename std::enable_if<std::is_integral<_conv>::value>::type from_string(_conv& value, std::basic_string<_char, _traits, _alloc> text)
	{
		_ljh::from_string(value, text.data(), text.size());
	}

	template<typename _conv, typename _char, size_t _size>
	typename std::enable_if<std::is_integral<_conv>::value && is_char_type<std::decay_t<_char>>::value>::type from_string(_conv& value, _char text[_size])
	{
		_ljh::from_string(value, text, _size);
	}

	template<typename _conv, typename _char>
	typename std::enable_if<std::is_integral<_conv>::value && is_char_type<std::decay_t<_char>>::value>::type from_string(_conv& value, _char* text)
	{
		_char* curr; for (curr = text; *curr != 0; curr++) {}
		_ljh::from_string(value, text, (curr - text)/sizeof(_char));
	}
}