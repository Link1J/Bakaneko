
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// int_types.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++11
// 
// ABOUT
//     Adds integer and floating point types in a style I like.
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#include <type_traits>

namespace ljh
{
	inline namespace int_types
	{
		using s08 =   signed char     ;
		using s16 =   signed short    ;
		using s32 =   signed int      ;
		using s64 =   signed long long;
		using u08 = unsigned char     ;
		using u16 = unsigned short    ;
		using u32 = unsigned int      ;
		using u64 = unsigned long long;
		using f32 =          float    ;
		using f64 =          double   ;
	}
}

// Size checks
static_assert(sizeof(ljh::s08) == 1, "Type 'ljh::int_types::s08' is not 1 byte large. Please correct this file for your platform." );
static_assert(sizeof(ljh::s16) == 2, "Type 'ljh::int_types::s16' is not 2 bytes large. Please correct this file for your platform.");
static_assert(sizeof(ljh::s32) == 4, "Type 'ljh::int_types::s32' is not 4 bytes large. Please correct this file for your platform.");
static_assert(sizeof(ljh::s64) == 8, "Type 'ljh::int_types::s64' is not 8 bytes large. Please correct this file for your platform.");
static_assert(sizeof(ljh::u08) == 1, "Type 'ljh::int_types::u08' is not 1 byte large. Please correct this file for your platform." );
static_assert(sizeof(ljh::u16) == 2, "Type 'ljh::int_types::u16' is not 2 bytes large. Please correct this file for your platform.");
static_assert(sizeof(ljh::u32) == 4, "Type 'ljh::int_types::u32' is not 4 bytes large. Please correct this file for your platform.");
static_assert(sizeof(ljh::u64) == 8, "Type 'ljh::int_types::u64' is not 8 bytes large. Please correct this file for your platform.");
static_assert(sizeof(ljh::f32) == 4, "Type 'ljh::int_types::f32' is not 4 bytes large. Please correct this file for your platform.");
static_assert(sizeof(ljh::f64) == 8, "Type 'ljh::int_types::f64' is not 8 bytes large. Please correct this file for your platform.");

static_assert(std::is_signed  <ljh::s08>::value, "Type 'ljh::int_types::s08' is not signed. Please correct this file for your platform."    );
static_assert(std::is_signed  <ljh::s16>::value, "Type 'ljh::int_types::s16' is not signed. Please correct this file for your platform."    );
static_assert(std::is_signed  <ljh::s32>::value, "Type 'ljh::int_types::s32' is not signed. Please correct this file for your platform."    );
static_assert(std::is_signed  <ljh::s64>::value, "Type 'ljh::int_types::s64' is not signed. Please correct this file for your platform."    );
static_assert(std::is_unsigned<ljh::u08>::value, "Type 'ljh::int_types::u08' is not unsigned. Please correct this file for your platform."  );
static_assert(std::is_unsigned<ljh::u16>::value, "Type 'ljh::int_types::u16' is not unsigned. Please correct this file for your platform."  );
static_assert(std::is_unsigned<ljh::u32>::value, "Type 'ljh::int_types::u32' is not unsigned. Please correct this file for your platform."  );
static_assert(std::is_unsigned<ljh::u64>::value, "Type 'ljh::int_types::u64' is not unsigned. Please correct this file for your platform."  );
static_assert(std::is_signed  <ljh::f32>::value, "Type 'ljh::int_types::f32' is not signed. Please report the problem to the compiler devs.");
static_assert(std::is_signed  <ljh::f64>::value, "Type 'ljh::int_types::f64' is not signed. Please report the problem to the compiler devs.");
