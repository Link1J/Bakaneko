
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// compile_time_string.hpp - v1.1
// SPDX-License-Identifier: BSL-1.0
//
// Requires C++20
// Requires MVSC 19.23 or GCC 10 or clang 10
//
// ABOUT
//     Adds some basic concepts
//
// Version History
//     1.0 Inital Version

#pragma once
#include "cpp_version.hpp"
#if LJH_CPP_VERSION < LJH_CPP17_VERSION || !(__cpp_concepts > 0)
#error "C++20 concepts are needed for this header"
#endif

#include <concepts>
#include <type_traits>
#include "type_traits.hpp"

namespace ljh
{
	template <typename T, typename... Args>
	concept one_of = (... || std::same_as<T, Args>);

	template <typename T>
	concept char_type = one_of<T, 
		char,
		signed char,
		unsigned char,
		wchar_t,
#if __cpp_char8_t >= 201811L
		char8_t,
#endif
		char16_t,
		char32_t
	>;

	template<typename T>
	concept scoped_enum = is_scoped_enum_v<T>;
}