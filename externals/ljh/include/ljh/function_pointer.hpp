
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// function_pointer.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++11
// 
// ABOUT
//     Adds function_pointer - like std::function but for pointers
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#include "function_traits.hpp"
#include "int_types.hpp"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4348)
#endif

#if LJH_CPP_VERSION >= LJH_CPP17_VERSION && __has_include(<string_view>)
#include <string_view>
#define _string_type std::string_view
#else
#include <string>
#define _string_type std::string
#endif

#ifdef LJH_FUNCTION_POINTERS_LOAD_RETURNS_EXPECTED
#if LJH_CPP_VERSION < LJH_CPP17_VERSION
#error "LJH_FUNCTION_POINTERS_LOAD_RETURNS_EXPECTED needs C++17 or newer"
#undef LJH_FUNCTION_POINTERS_LOAD_RETURNS_EXPECTED
#else
#include "expected.hpp"
#endif
#endif

namespace ljh
{
	template<typename T, int = 0>
	class function_pointer;

#define POINTERS_INTERALS(CC, Noexcept)\
	template<typename R, typename... Args>\
	class function_pointer<\
		R LJH_CALLING_CONVENTION_##CC (Args...) noexcept(Noexcept),\
		std::is_same<calling_conventions::cdecl_::type, calling_conventions::CC##_::type>::value\
			? calling_conventions::CC##_::id \
			: 0 \
	>\
	{\
	public:\
		using traits = function_traits<R LJH_CALLING_CONVENTION_##CC (Args...) noexcept(Noexcept)>;\
		using type = typename traits::as::function_pointer;\
\
	private:\
		using noexcept_t = function_traits<R LJH_CALLING_CONVENTION_##CC (Args...) noexcept(!Noexcept)>;\
		type function = nullptr;\
\
		template<typename T, int = sizeof(function_traits<T>)>\
		struct enable_function_pointer;\
\
		template<typename T>\
		struct enable_function_pointer<T, 1>\
		{\
			using ref = typename std::enable_if<function_traits<T>::is::function_pointer, function_pointer&>::type;\
			using boo = typename std::enable_if<function_traits<T>::is::function_pointer, bool             >::type;\
		};\
\
	public:\
		constexpr function_pointer() noexcept = default;\
		constexpr          function_pointer(type      other) noexcept : function{                       other } {}\
		constexpr explicit function_pointer(uintptr_t other) noexcept : function{reinterpret_cast<type>(other)} {}\
		constexpr explicit function_pointer(void*     other) noexcept : function{reinterpret_cast<type>(other)} {}\
		template<typename T, typename = typename enable_function_pointer<T>::ref>\
		constexpr explicit function_pointer(T         other) noexcept : function{reinterpret_cast<type>(other)} {}\
\
		function_pointer& operator=(uintptr_t other) noexcept { function = reinterpret_cast<type>(other); return *this; }\
		function_pointer& operator=(void*     other) noexcept { function = reinterpret_cast<type>(other); return *this; }\
		template<typename T> typename enable_function_pointer<T>::ref \
		                  operator=(T         other) noexcept { function = reinterpret_cast<type>(other); return *this; }\
\
		bool operator==(type      other) const noexcept { return function ==                        other ; }\
		bool operator==(uintptr_t other) const noexcept { return function == reinterpret_cast<type>(other); }\
		bool operator==(void*     other) const noexcept { return function == reinterpret_cast<type>(other); }\
		template<typename T> typename enable_function_pointer<T>::boo \
		     operator==(T         other) const noexcept { return function == reinterpret_cast<type>(other); }\
\
		bool operator!=(type      other) const noexcept { return !(*this == other); }\
		bool operator!=(uintptr_t other) const noexcept { return !(*this == other); }\
		bool operator!=(void*     other) const noexcept { return !(*this == other); }\
		template<typename T> typename enable_function_pointer<T>::boo \
		     operator!=(T         other) const noexcept { return !(*this == other); }\
\
		R operator()(Args... args) const noexcept(traits::is::no_exceptions) { return function(args...); }\
		type get() const noexcept { return function; }\
		bool empty() const noexcept { return function == nullptr; }\
\
		         operator bool     () const noexcept { return !empty(); }\
		         operator type     () const noexcept { return            get(); }\
		explicit operator void*    () const noexcept { return (void*    )get(); }\
		explicit operator uintptr_t() const noexcept { return (uintptr_t)get(); }\
	}

#if __cpp_noexcept_function_type >= 201510L
#define MAKE_POINTERS(CC)\
	POINTERS_INTERALS(CC, false);\
	POINTERS_INTERALS(CC, true )
#else
#define MAKE_POINTERS(CC)\
	POINTERS_INTERALS(CC, false)
#endif

	MAKE_POINTERS(cdecl             );
	MAKE_POINTERS(stdcall           );
	MAKE_POINTERS(fastcall          );
	MAKE_POINTERS(vectorcall        );
	MAKE_POINTERS(regcall           );
	MAKE_POINTERS(ms_abi            );
	MAKE_POINTERS(preserve_all      );
	MAKE_POINTERS(preserve_most     );
//	MAKE_POINTERS(thiscall          );
	MAKE_POINTERS(aarch64_vector_pcs);

#undef MAKE_POINTERS
#undef POINTERS_INTERALS

	bool _load_dll(const char* dll_name, const char* function_name, void** function, u32* error);

#ifndef LJH_FUNCTION_POINTERS_LOAD_RETURNS_EXPECTED
	template<typename function_type>
	function_pointer<function_type> load_function(_string_type dll_name, _string_type function_name)
	{
		void* function; u32 error;
		_load_dll(dll_name.data(), function_name.data(), &function, &error);
		return function_pointer<function_type>{function};
	}
	template<typename function_type>
	function_pointer<function_type> load_function(_string_type dll_name, u16 ordinal)
	{
		void* function; u32 error;
		_load_dll(dll_name.data(), (const char*)(ordinal), &function, &error);
		return function_pointer<function_type>{function};
	}
#else
	template<typename function_type, typename _char = char>
	expected<function_pointer<function_type>,u32> load_function(_string_type dll_name, _string_type function_name)
	{
		void* function; u32 error;
		if (!_load_dll(dll_name.data(), function_name.data(), &function, &error))
			return unexpected{error};
		return function_pointer<function_type>{function};
	}
	template<typename function_type, typename _char = char>
	expected<function_pointer<function_type>,u32> load_function(_string_type dll_name, u16 ordinal)
	{
		void* function; u32 error;
		if (!_load_dll(dll_name.data(), (const char*)(ordinal), &function, &error))
			return unexpected{error};
		return function_pointer<function_type>{function};
	}
#endif
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#undef _string_type

static_assert(sizeof(ljh::function_pointer<void()>) == sizeof(void(*)()), "ljh::function_pointer and a function pointer must be the same size");