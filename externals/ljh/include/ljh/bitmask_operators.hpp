
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// bitmask_operators.hpp - v1.2
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// Requires is_scoped_enum.hpp and casting.hpp
// 
// Based on code from https://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
// 
// ABOUT
//     Used for making scoped enums into bitmasks
//
// USAGE
//     Add 'template<> struct ljh::bitmask_operators::enable<your_type> : std::true_type {};'
//     replacing "your_type" with the type in the header file for that type. 
//
// Version History
//     1.0 Inital Version
//     1.1 Add namespace to 'static_cast_underlying' and cleanup the |= operator
//     1.2 Using more C++17 stuff

#pragma once
#include "type_traits.hpp"
#include "casting.hpp"

namespace ljh::bitmask_operators
{
	template<typename T>
	struct enable : std::false_type {};
	template<typename T> 
	using enable_t = typename std::enable_if_t<enable<T>::value, T>;
}

template<typename T> 
constexpr ljh::bitmask_operators::enable_t<T> operator|(T lhs, T rhs) 
{
	static_assert(ljh::is_scoped_enum<T>::value, "Type is not a scoped enum.");
	return static_cast<T>(ljh::underlying_cast(lhs) | ljh::underlying_cast(rhs)); 
}

template<typename T> 
constexpr ljh::bitmask_operators::enable_t<T> operator&(T lhs, T rhs) 
{
	static_assert(ljh::is_scoped_enum<T>::value, "Type is not a scoped enum.");
	return static_cast<T>(ljh::underlying_cast(lhs) & ljh::underlying_cast(rhs)); 
}

template<typename T> 
constexpr ljh::bitmask_operators::enable_t<T> operator^(T lhs, T rhs) 
{
	static_assert(ljh::is_scoped_enum<T>::value, "Type is not a scoped enum.");
	return static_cast<T>(ljh::underlying_cast(lhs) ^ ljh::underlying_cast(rhs)); 
}

template<typename T> 
constexpr ljh::bitmask_operators::enable_t<T> operator~(T rhs) 
{
	static_assert(ljh::is_scoped_enum<T>::value, "Type is not a scoped enum.");
	return static_cast<T>(~ljh::underlying_cast(rhs)); 
}

template<typename T> 
constexpr ljh::bitmask_operators::enable_t<T>& operator|=(T& lhs, T rhs) 
{
	lhs = lhs | rhs; 
	return lhs;
}

template<typename T> 
constexpr ljh::bitmask_operators::enable_t<T>& operator&=(T& lhs, T rhs) 
{
	lhs = lhs & rhs; 
	return lhs;
}

template<typename T> 
constexpr ljh::bitmask_operators::enable_t<T>& operator^=(T& lhs, T rhs) 
{
	lhs = lhs ^ rhs; 
	return lhs;
}