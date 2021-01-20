
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// casting.hpp - v1.2
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++11
// 
// ABOUT
//     Casting functions
//
// USAGE
//
// Version History
//     1.0 Inital Version
//     1.1 Rename casting.hpp and add reinterpret
//     1.2 Add pointer_cast and dereference_as, Rename static_cast_underlying to underlying_cast

#pragma once

#include <type_traits>

namespace ljh
{
	template<typename T> 
	constexpr typename std::underlying_type<T>::type underlying_cast(T a) 
	{
		return static_cast<typename std::underlying_type<T>::type>(a); 
	};

	template<typename out, typename in>
	out* pointer_cast(in* value)
	{
		return (out*)(value);
	}

	template<typename out, typename in>
	out reinterpret(in&& value)
	{
		return *pointer_cast<out>(&value);
	}

	template<typename out, typename in>
	out dereference_as(in* value)
	{
		return *pointer_cast<out>(value);
	}
}