
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// type_traits.hpp - v1.3
// SPDX-License-Identifier: BSL-1.0
//
// Requires C++11
// Requires cpp_version.hpp
//
// Based on code from https://stackoverflow.com/a/10724828/13155694
// Based on code from https://stackoverflow.com/a/15396757/7932993
//
// ABOUT
//     Adds extra type traits
//
// USAGE
//
// Version History
//     1.0 Inital Version
//     1.1 Add is_char and rename to type_traits.hpp
//     1.2 Change is_char to is_char_type
//     1.3 Add is_callable and is_function_pointer

#pragma once

#include "cpp_version.hpp"
#include <type_traits>

namespace ljh
{
	template <typename T, bool = std::is_enum<T>::value>
	struct is_scoped_enum : std::false_type {};

	template <typename T>
	struct is_scoped_enum<T, true> : std::integral_constant<bool, !std::is_convertible<T, typename std::underlying_type<T>::type>::value> {};

	template<typename T>
	struct is_char_type : std::false_type {};

	template<> struct is_char_type<         char    > : std::true_type {};
	template<> struct is_char_type<signed   char    > : std::true_type {};
	template<> struct is_char_type<unsigned char    > : std::true_type {};
	template<> struct is_char_type<        wchar_t  > : std::true_type {};
#if defined(__cpp_char8_t)
	template<> struct is_char_type<         char8_t > : std::true_type {};
#endif
	template<> struct is_char_type<         char16_t> : std::true_type {};
	template<> struct is_char_type<         char32_t> : std::true_type {};

	template<typename T>
	struct is_callable
	{
	private:
		typedef char(&yes)[1];
		typedef char(&no)[2];

		struct Fallback { void operator()(); };
		struct Derived : T, Fallback { };

		template<typename U, U> struct Check;

		template<typename>
		static yes test(...);

		template<typename C>
		static no test(Check<void (Fallback::*)(), &C::operator()>*);

	public:
		static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
	};

	template<typename T> 
	struct is_function_pointer : std::integral_constant<bool, std::is_pointer<T>::value && std::is_function<typename std::remove_pointer<T>::type>::value> {};

	template <typename, template <typename...> typename>
	struct _is_instance_impl : public std::false_type {};

	template <template <typename...> typename U, typename...Ts>
	struct _is_instance_impl<U<Ts...>, U> : public std::true_type {};

	template <typename T, template <typename ...> typename U>
	using is_instance = _is_instance_impl<std::decay_t<T>, U>;
	
#if LJH_CPP_VERSION >= LJH_CPP17_VERSION
	template <typename T>
	inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
	
	template<typename T>
	inline constexpr bool is_char_type_v = is_char_type<T>::value;

	template<typename T>
	inline constexpr bool is_callable_v = is_callable<T>::value;
	
	template<typename T>
	inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;

	template <typename T, template <typename ...> typename U>
	inline constexpr bool is_instance_v = is_instance<T, U>::value;
#endif

#if LJH_CPP_VERSION > LJH_CPP17_VERSION
	template<typename T> using remove_cvref   = std::remove_cvref  <T>;
	template<typename T> using remove_cvref_t = std::remove_cvref_t<T>;
#else
	template< class T >
	struct remove_cvref
	{
		using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
	};
	
#if LJH_CPP_VERSION >= LJH_CPP14_VERSION
	template< class T >
	using remove_cvref_t = typename remove_cvref<T>::type;
#endif
#endif
}