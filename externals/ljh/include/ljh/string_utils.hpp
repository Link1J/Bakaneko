
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// string_utils.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++11
// 
// ABOUT
//     String utils
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#include "cpp_version.hpp"

#include <string>
#include <vector>
#include <algorithm>

#if __cpp_lib_string_view >= 201606L
#include <string_view>
#include "get_index.hpp"
#endif

namespace ljh
{
	template<class C, class T = std::char_traits<C>, class A = std::allocator<C>>
	void ltrim(std::basic_string<C,T,A> &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](C ch) {
			return !std::isspace(ch);
		}));
	}

	template<class C, class T = std::char_traits<C>, class A = std::allocator<C>>
	void rtrim(std::basic_string<C,T,A> &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](C ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	template<class C, class T = std::char_traits<C>, class A = std::allocator<C>>
	void trim(std::basic_string<C,T,A> &s)
	{
		ltrim(s);
		rtrim(s);
	}

	template<class C, class T = std::char_traits<C>, class A = std::allocator<C>>
	std::basic_string<C,T,A> ltrim_copy(std::basic_string<C,T,A> s)
	{
		ltrim(s);
		return s;
	}

	template<class C, class T = std::char_traits<C>, class A = std::allocator<C>>
	std::basic_string<C,T,A> rtrim_copy(std::basic_string<C,T,A> s)
	{
		rtrim(s);
		return s;
	}

	template<class C, class T = std::char_traits<C>, class A = std::allocator<C>>
	std::basic_string<C,T,A> trim_copy(std::basic_string<C,T,A> s)
	{
		trim(s);
		return s;
	}

	template<class C, class T = std::char_traits<C>, class A = std::allocator<C>>
	std::vector<std::basic_string<C,T,A>> split(const std::basic_string<C,T,A>& s, C seperator)
	{
		using size_type = typename std::basic_string<C,T,A>::size_type;

		std::vector<std::basic_string<C,T,A>> output;
		size_type prev_pos = 0, pos = 0;

		while((pos = s.find(seperator, pos)) != std::basic_string<C,T,A>::npos)
		{
			output.push_back(s.substr(prev_pos, pos - prev_pos));
			prev_pos = ++pos;
		}

		output.push_back(s.substr(prev_pos, pos-prev_pos));
		return output;
	}

#if __cpp_lib_string_view >= 201606L
	template<class C, class T = std::char_traits<C>>
	void ltrim(std::basic_string_view<C,T> &s)
	{
		s.remove_prefix(get_index_if(s.begin(), s.end(), [](C ch) {
			return !std::isspace(ch);
		}));
	}

	template<class C, class T = std::char_traits<C>>
	void rtrim(std::basic_string_view<C,T> &s)
	{
		s.remove_suffix(get_index_if(s.rbegin(), s.rend(), [](C ch) {
			return !std::isspace(ch);
		}));
	}

	template<class C, class T = std::char_traits<C>>
	void trim(std::basic_string_view<C,T> &s)
	{
		ltrim(s);
		rtrim(s);
	}

	template<class C, class T = std::char_traits<C>>
	std::basic_string_view<C,T> ltrim_copy(std::basic_string_view<C,T> s)
	{
		ltrim(s);
		return s;
	}

	template<class C, class T = std::char_traits<C>>
	std::basic_string_view<C,T> rtrim_copy(std::basic_string_view<C,T> s)
	{
		rtrim(s);
		return s;
	}

	template<class C, class T = std::char_traits<C>>
	std::basic_string_view<C,T> trim_copy(std::basic_string_view<C,T> s)
	{
		trim(s);
		return s;
	}

	template<class C, class T = std::char_traits<C>>
	std::vector<std::basic_string_view<C,T>> split(const std::basic_string_view<C,T>& s, C seperator)
	{
		using size_type = typename std::basic_string_view<C,T>::size_type;
		
		std::vector<std::basic_string_view<C,T>> output;
		size_type::size_type prev_pos = 0, pos = 0;

		while((pos = s.find(seperator, pos)) != std::basic_string_view<C,T>::npos)
		{
			output.push_back(s.substr(prev_pos, pos - prev_pos));
			prev_pos = ++pos;
		}

		output.push_back(s.substr(prev_pos, pos-prev_pos));
		return output;
	}
#endif
}