
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// cpp_version.hpp - v1.2
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++
// 
// ABOUT
//     Adds support for version checking marcos and add C++ Version info
//
// USAGE
//
// Version History
//     1.0 Inital Version
//     1.1 Add more constexpr versions and LJH_TRY_TO_GET_FEATURE_TESTING
//     1.2 Add C++ 20 version, add compiler macros

#pragma once

#ifdef _MSVC_LANG
#	define LJH_CPP_VERSION _MSVC_LANG
#else
#	define LJH_CPP_VERSION __cplusplus
#endif

#define LJH_CPP11_VERSION 201103L
#define LJH_CPP14_VERSION 201402L
#define LJH_CPP17_VERSION 201703L
#define LJH_CPP20_VERSION 202002L

#define LJH_COMPILER_GEN_FULL_VERSION(ma, mi, pa) (ma * 10000 + mi * 100 + pa)
#define LJH_COMPILER_GEN_VERSION(ma, mi) (ma * 100 + mi)

#if defined(__clang__)
#	define LJH_COMPILER_CLANG
#	if defined(__apple_build_version__)
#		define LJH_COMPILER_APPLE_CLANG
#	endif
#	define LJH_CLANG_FULL_VERSION LJH_COMPILER_GEN_FULL_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#	define LJH_CLANG_VERSION LJH_COMPILER_GEN_VERSION(__clang_major__, __clang_minor__)
#elif defined(__GNUC__)
#	define LJH_COMPILER_GCC
#	define LJH_GCC_FULL_VERSION LJH_COMPILER_GEN_FULL_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#	define LJH_GCC_VERSION LJH_COMPILER_GEN_VERSION(__GNUC__, __GNUC_MINOR__)
#elif defined(_MSC_VER)
#	define LJH_COMPILER_MSVC
#	define LJH_MSVC_FULL_VERSION _MSC_FULL_VER
#	define LJH_MSVC_VERSION _MSC_VER
#endif

#if LJH_CPP_VERSION > LJH_CPP17_VERSION
#	if __has_include(<version>)
#		include <version>
#	endif
#endif

#if defined(LJH_TRY_TO_GET_FEATURE_TESTING)
#	if !(defined(_STD_VERSION_HEADER_) || defined(_GLIBCXX_VERSION_INCLUDED) || defined(_LIBCPP_VERSIONH))
#		if _MSC_VER
#			include <yvals_core.h>
#		else
#			include <ciso646>
#			if defined(__GLIBCXX__)
#				include <bits/stdc++.h>
#			else
#				if __has_include(<cassert>)
#					include <cassert>
#				endif
#				if __has_include(<cctype>)
#					include <cctype>
#				endif
#				if __has_include(<cerrno>)
#					include <cerrno>
#				endif
#				if __has_include(<cfloat>)
#					include <cfloat>
#				endif
#				if __has_include(<ciso646>)
#					include <ciso646>
#				endif
#				if __has_include(<climits>)
#					include <climits>
#				endif
#				if __has_include(<clocale>)
#					include <clocale>
#				endif
#				if __has_include(<cmath>)
#					include <cmath>
#				endif
#				if __has_include(<csetjmp>)
#					include <csetjmp>
#				endif
#				if __has_include(<csignal>)
#					include <csignal>
#				endif
#				if __has_include(<cstdarg>)
#					include <cstdarg>
#				endif
#				if __has_include(<cstddef>)
#					include <cstddef>
#				endif
#				if __has_include(<cstdio>)
#					include <cstdio>
#				endif
#				if __has_include(<cstdlib>)
#					include <cstdlib>
#				endif
#				if __has_include(<cstring>)
#					include <cstring>
#				endif
#				if __has_include(<ctime>)
#					include <ctime>
#				endif
#				if __has_include(<cwchar>)
#					include <cwchar>
#				endif
#				if __has_include(<cwctype>)
#					include <cwctype>
#				endif
#				if __has_include(<algorithm>)
#					include <algorithm>
#				endif
#				if __has_include(<bitset>)
#					include <bitset>
#				endif
#				if __has_include(<complex>)
#					include <complex>
#				endif
#				if __has_include(<deque>)
#					include <deque>
#				endif
#				if __has_include(<exception>)
#					include <exception>
#				endif
#				if __has_include(<fstream>)
#					include <fstream>
#				endif
#				if __has_include(<functional>)
#					include <functional>
#				endif
#				if __has_include(<iomanip>)
#					include <iomanip>
#				endif
#				if __has_include(<ios>)
#					include <ios>
#				endif
#				if __has_include(<iosfwd>)
#					include <iosfwd>
#				endif
#				if __has_include(<iostream>)
#					include <iostream>
#				endif
#				if __has_include(<istream>)
#					include <istream>
#				endif
#				if __has_include(<iterator>)
#					include <iterator>
#				endif
#				if __has_include(<limits>)
#					include <limits>
#				endif
#				if __has_include(<list>)
#					include <list>
#				endif
#				if __has_include(<locale>)
#					include <locale>
#				endif
#				if __has_include(<map>)
#					include <map>
#				endif
#				if __has_include(<memory>)
#					include <memory>
#				endif
#				if __has_include(<new>)
#					include <new>
#				endif
#				if __has_include(<numeric>)
#					include <numeric>
#				endif
#				if __has_include(<ostream>)
#					include <ostream>
#				endif
#				if __has_include(<queue>)
#					include <queue>
#				endif
#				if __has_include(<set>)
#					include <set>
#				endif
#				if __has_include(<sstream>)
#					include <sstream>
#				endif
#				if __has_include(<stack>)
#					include <stack>
#				endif
#				if __has_include(<stdexcept>)
#					include <stdexcept>
#				endif
#				if __has_include(<streambuf>)
#					include <streambuf>
#				endif
#				if __has_include(<string>)
#					include <string>
#				endif
#				if __has_include(<typeinfo>)
#					include <typeinfo>
#				endif
#				if __has_include(<utility>)
#					include <utility>
#				endif
#				if __has_include(<valarray>)
#					include <valarray>
#				endif
#				if __has_include(<vector>)
#					include <vector>
#				endif
#				if __has_include(<ccomplex>)
#					include <ccomplex>
#				endif
#				if __has_include(<cfenv>)
#					include <cfenv>
#				endif
#				if __has_include(<cinttypes>)
#					include <cinttypes>
#				endif
#				if __has_include(<cstdalign>)
#					include <cstdalign>
#				endif
#				if __has_include(<cstdbool>)
#					include <cstdbool>
#				endif
#				if __has_include(<cstdint>)
#					include <cstdint>
#				endif
#				if __has_include(<ctgmath>)
#					include <ctgmath>
#				endif
#				if __has_include(<cuchar>)
#					include <cuchar>
#				endif
#				if __has_include(<array>)
#					include <array>
#				endif
#				if __has_include(<atomic>)
#					include <atomic>
#				endif
#				if __has_include(<chrono>)
#					include <chrono>
#				endif
#				if __has_include(<codecvt>)
#					include <codecvt>
#				endif
#				if __has_include(<condition_variable>)
#					include <condition_variable>
#				endif
#				if __has_include(<forward_list>)
#					include <forward_list>
#				endif
#				if __has_include(<future>)
#					include <future>
#				endif
#				if __has_include(<initializer_list>)
#					include <initializer_list>
#				endif
#				if __has_include(<mutex>)
#					include <mutex>
#				endif
#				if __has_include(<random>)
#					include <random>
#				endif
#				if __has_include(<ratio>)
#					include <ratio>
#				endif
#				if __has_include(<regex>)
#					include <regex>
#				endif
#				if __has_include(<scoped_allocator>)
#					include <scoped_allocator>
#				endif
#				if __has_include(<system_error>)
#					include <system_error>
#				endif
#				if __has_include(<thread>)
#					include <thread>
#				endif
#				if __has_include(<tuple>)
#					include <tuple>
#				endif
#				if __has_include(<typeindex>)
#					include <typeindex>
#				endif
#				if __has_include(<type_traits>)
#					include <type_traits>
#				endif
#				if __has_include(<unordered_map>)
#					include <unordered_map>
#				endif
#				if __has_include(<unordered_set>)
#					include <unordered_set>
#				endif
#				if __has_include(<shared_mutex>)
#					include <shared_mutex>
#				endif
#				if __has_include(<any>)
#					include <any>
#				endif
#				if __has_include(<charconv>)
#					include <charconv>
#				endif
#				if __has_include(<execution>)
#					include <execution>
#				endif
#				if __has_include(<filesystem>)
#					include <filesystem>
#				endif
#				if __has_include(<optional>)
#					include <optional>
#				endif
#				if __has_include(<memory_resource>)
#					include <memory_resource>
#				endif
#				if __has_include(<string_view>)
#					include <string_view>
#				endif
#				if __has_include(<variant>)
#					include <variant>
#				endif
#				if __has_include(<bit>)
#					include <bit>
#				endif
#				if __has_include(<compare>)
#					include <compare>
#				endif
#				if __has_include(<concepts>)
#					include <concepts>
#				endif
#				if __has_include(<numbers>)
#					include <numbers>
#				endif
#				if __has_include(<ranges>)
#					include <ranges>
#				endif
#				if __has_include(<span>)
#					include <span>
#				endif
#				if __has_include(<stop_token>)
#					include <stop_token>
#				endif
#				if __has_include(<syncstream>)
#					include <syncstream>
#				endif
#			endif
#		endif
#	endif

#	if !defined(__cpp_static_assert)
#		if defined(_MSC_VER)
/	/#		if LJH_CPP_VERSION >= LJH_CPP11_VERSION  // MSVC has no flags for C++11
#				if _MSC_VER >= 1600
#					define __cpp_static_assert 200410L
#					define __cpp_lambdas 200907L
#					define __cpp_decltype 200707L
#					define __cpp_threadsafe_static_init 200806L
#					define __cpp_rvalue_references 200610L
#				endif
#				if _MSC_VER >= 1700
#					define __cpp_range_based_for 200907L
#				endif
#				if _MSC_VER >= 1800
#					define __cpp_alias_templates 200704L
#					define __cpp_variadic_templates 200704L
#					define __cpp_raw_strings 200710L
#					define __cpp_initializer_lists 200806L
#					define __cpp_delegating_constructors 200604L
#					define __cpp_nsdmi 200809L
#					define __cpp_lib_transparent_operators 201210L // This is C++14 but predates /std:c++14 support
#				endif
#				if _MSC_VER >= 1900
#					define __cpp_constexpr 200704L
#					define __cpp_ref_qualifiers 200710L
#					define __cpp_attributes 200809L
#					define __cpp_unicode_characters 200704L
#					define __cpp_unicode_literals 200710L
#					define __cpp_user_defined_literals 200809L
#					define __cpp_inheriting_constructors 200802L
#				endif
/	/#		endif
#			if LJH_CPP_VERSION >= LJH_CPP14_VERSION
#				if _MSC_VER >= 1900
#				endif
#			endif
#		elif defined(__GNUC__)
#			if LJH_CPP_VERSION >= LJH_CPP11_VERSION
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,3,0)
#					define __cpp_static_assert 200410L
#					define __cpp_rvalue_references 200610L
#					define __cpp_decltype 200707L
#					define __cpp_variadic_templates 200704L
#					define __cpp_threadsafe_static_init 200806L
#				endif
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,4,0)
#					define __cpp_initializer_lists 200806L
#					define __cpp_unicode_characters 200704L
#					define __cpp_unicode_literals 200710L
#				endif
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,5,0)
#					define __cpp_lambdas 200907L
#					define __cpp_raw_strings 200710L
#				endif
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,6,0)
#					define __cpp_range_based_for 200907L
#					define __cpp_constexpr 200704L
#				endif
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,7,0)
#					define __cpp_user_defined_literals 200809L
#					define __cpp_delegating_constructors 200604L
#					define __cpp_nsdmi 200809L
#				endif
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,8,0)
#					define __cpp_inheriting_constructors 200802L
#					define __cpp_attributes 200809L
#				endif
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,8,1)
#					define __cpp_ref_qualifiers 200710L
#				endif
#			endif
#			if LJH_CPP_VERSION >= LJH_CPP14_VERSION
#				if LJH_GCC_VERSION > LJH_GEN_GCC_VERSION(4,9,0)
#					define __cpp_lib_transparent_operators 201210L
#					define __cpp_lib_make_unique 201304L
#				endif
#			endif
#		else
#			error "Unknown compiler."
#		endif
#	endif
#endif

#if __cpp_inline_variables >= 201606L
#define LJH_CPP17_INLINE_VAR inline
#else
#define LJH_CPP17_INLINE_VAR
#endif

#if __cpp_constexpr >= 200704L
#define LJH_CPP11_CONSTEXPR constexpr
#else
#define LJH_CPP11_CONSTEXPR
#endif

#if __cpp_constexpr >= 201304L
#define LJH_CPP14_CONSTEXPR constexpr
#else
#define LJH_CPP14_CONSTEXPR
#endif

#if __cpp_constexpr >= 201603L
#define LJH_CPP17_CONSTEXPR constexpr
#else
#define LJH_CPP17_CONSTEXPR
#endif

#if __cpp_constexpr >= 201907L
#define LJH_CPP20_CONSTEXPR constexpr
#else
#define LJH_CPP20_CONSTEXPR
#endif

#if __cpp_if_constexpr >= 201606L
#define LJH_IF_CONSTEXPR if constexpr
#else
#define LJH_IF_CONSTEXPR if
#endif

#if __cpp_noexcept_function_type >= 201510L
#define LJH_NOEXCEPT_FUNCTION_TYPE noexcept
#else
#define LJH_NOEXCEPT_FUNCTION_TYPE(A)
#endif

#if __cpp_conditional_explicit >= 201806L
#define LJH_CPP20_EXPLICIT explicit
#else
#define LJH_CPP20_EXPLICIT(...)
#endif
