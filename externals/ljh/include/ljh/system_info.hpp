
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// system_info.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// 
// ABOUT
//     System Info Collection
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#include "version.hpp"
#include "expected.hpp"
#include "int_types.hpp"
#include "os_build_info.hpp"

namespace ljh::system_info
{
	enum class error
	{
		unknown_os,
		cannot_get_version,
		no_info,
	};

	enum class platform
	{
		Windows,
		Unix,
		macOS,
		iOS,
		Linux,
		Android,
		WindowsDOS,

		WindowsNT = Windows,
		Mac_OS_X  = macOS,
		OS_X      = macOS,
		Mac_OS    = macOS,
		iPhone_OS = iOS,
		iPadOS    = iOS,
	};
	struct info_data
	{
		ljh::system_info::platform platform;
		ljh::version               version ;
		u32                        sdk     ;

		bool operator==(const ljh::system_info::platform&) const;
		bool operator==(const ljh::version              &) const;
		bool operator!=(const ljh::version              &) const;
		bool operator< (const ljh::version              &) const;
		bool operator> (const ljh::version              &) const;
		bool operator>=(const ljh::version              &) const;
		bool operator<=(const ljh::version              &) const;
		bool operator==(const ljh::u32                  &) const;
		bool operator!=(const ljh::u32                  &) const;
		bool operator< (const ljh::u32                  &) const;
		bool operator> (const ljh::u32                  &) const;
		bool operator>=(const ljh::u32                  &) const;
		bool operator<=(const ljh::u32                  &) const;
	};

	expected<platform   , error> get_platform    (); // Use for Version Check
	expected<version    , error> get_version     (); // Use for Version Check
	expected<std::string, error> get_string      (); // Only use for Logs
	expected<u32        , error> get_sdk         (); // An infinitely increasing number
	expected<std::string, error> get_model       (); // Only use for Logs
	expected<std::string, error> get_manufacturer(); // Only use for Logs

	// OS Versions
	inline namespace versions
	{
		extern const info_data Windows_1_01   ; extern const info_data Windows_1_0              ;
		extern const info_data Windows_1_02   ;
		extern const info_data Windows_1_03   ;
		extern const info_data Windows_1_04   ;
		extern const info_data Windows_2_03   ; extern const info_data Windows_2_0              ;
		extern const info_data Windows_2_10   ;
		extern const info_data Windows_2_11   ;
		extern const info_data Windows_3_0    ;
		extern const info_data Windows_3_1    ;
		extern const info_data Windows_3_11   ;
		extern const info_data Windows_95     ;
		extern const info_data Windows_98     ;
		extern const info_data Windows_98SE   ;
		extern const info_data Windows_ME     ;
		extern const info_data Windows_NT_3_1 ;
		                                        extern const info_data Windows_NT_3_1_SP_1      ;
		                                        extern const info_data Windows_NT_3_1_SP_2      ;
		                                        extern const info_data Windows_NT_3_1_SP_3      ;
		extern const info_data Windows_NT_3_5 ;
		                                        extern const info_data Windows_NT_3_5_SP_1      ;
		                                        extern const info_data Windows_NT_3_5_SP_2      ;
		                                        extern const info_data Windows_NT_3_5_SP_3      ;
		extern const info_data Windows_NT_3_51;
		                                        extern const info_data Windows_NT_3_51_SP_1     ;
		                                        extern const info_data Windows_NT_3_51_SP_2     ;
		                                        extern const info_data Windows_NT_3_51_SP_3     ;
		                                        extern const info_data Windows_NT_3_51_SP_4     ;
		                                        extern const info_data Windows_NT_3_51_SP_5     ;
		extern const info_data Windows_NT_4_0 ;
		                                        extern const info_data Windows_NT_4_0_SP_1      ;
		                                        extern const info_data Windows_NT_4_0_SP_2      ;
		                                        extern const info_data Windows_NT_4_0_SP_3      ;
		                                        extern const info_data Windows_NT_4_0_SP_4      ;
		                                        extern const info_data Windows_NT_4_0_SP_5      ;
		                                        extern const info_data Windows_NT_4_0_SP_6      ;
		                                        extern const info_data Windows_NT_4_0_SP_6a     ;
		extern const info_data Windows_NT_5_0 ; extern const info_data Windows_2000             ;
		                                        extern const info_data Windows_2000_SP_1        ;
		                                        extern const info_data Windows_2000_SP_2        ;
		                                        extern const info_data Windows_2000_SP_3        ;
		                                        extern const info_data Windows_2000_SP_4        ;
		extern const info_data Windows_NT_5_1 ; extern const info_data Windows_XP               ;
		                                        extern const info_data Windows_XP_SP_1          ;
		                                        extern const info_data Windows_XP_SP_2          ;
		                                        extern const info_data Windows_XP_SP_3          ;
		extern const info_data Windows_NT_5_2 ; extern const info_data Windows_XP_x64           ;
		                                        extern const info_data Windows_XP_x64_SP_3      ;
		extern const info_data Windows_NT_6_0 ; extern const info_data Windows_Vista            ;
		                                        extern const info_data Windows_Vista_SP_1       ;
		                                        extern const info_data Windows_Vista_SP_2       ;
		                                        extern const info_data Windows_Vista_SP_2_Update;
		extern const info_data Windows_NT_6_1 ; extern const info_data Windows_7                ;
		                                        extern const info_data Windows_7_SP_1           ;
		extern const info_data Windows_NT_6_2 ; extern const info_data Windows_8                ;
		extern const info_data Windows_NT_6_3 ; extern const info_data Windows_8_1              ;
		extern const info_data Windows_NT_10_0; extern const info_data Windows_10               ; extern const info_data Windows_10_1507           ;
		                                        extern const info_data Windows_10_1511          ;
		                                        extern const info_data Windows_10_1607          ;
		                                        extern const info_data Windows_10_1703          ;
		                                        extern const info_data Windows_10_1709          ;
		                                        extern const info_data Windows_10_1803          ;
		                                        extern const info_data Windows_10_1809          ;
		                                        extern const info_data Windows_10_1903          ;
		                                        extern const info_data Windows_10_1909          ;
		                                        extern const info_data Windows_10_2004          ;
		                                        extern const info_data Windows_10_20H2          ;
		extern const info_data Android_API_1  ; extern const info_data Android_1                ;
		extern const info_data Android_API_2  ; extern const info_data Android_1_1              ;
		extern const info_data Android_API_3  ; extern const info_data Android_1_5              ; extern const info_data Android_Cupcake           ;
		extern const info_data Android_API_4  ; extern const info_data Android_1_6              ; extern const info_data Android_Donut             ;
		extern const info_data Android_API_5  ; extern const info_data Android_2                ; extern const info_data Android_Eclair            ;
		extern const info_data Android_API_6  ; extern const info_data Android_2_0_1            ;
		extern const info_data Android_API_7  ; extern const info_data Android_2_1              ;
		extern const info_data Android_API_8  ; extern const info_data Android_2_2              ; extern const info_data Android_Froyo             ;
		extern const info_data Android_API_9  ; extern const info_data Android_2_3              ; extern const info_data Android_Gingerbread       ;
		extern const info_data Android_API_10 ; extern const info_data Android_2_3_3            ;
		extern const info_data Android_API_11 ; extern const info_data Android_3                ; extern const info_data Android_Honeycomb         ;
		extern const info_data Android_API_12 ; extern const info_data Android_3_1              ;
		extern const info_data Android_API_13 ; extern const info_data Android_3_2              ;
		extern const info_data Android_API_14 ; extern const info_data Android_4                ; extern const info_data Android_Ice_Cream_Sandwich;
		extern const info_data Android_API_15 ; extern const info_data Android_4_0_3            ;
		extern const info_data Android_API_16 ; extern const info_data Android_4_1              ; extern const info_data Android_Jelly_Bean        ;
		extern const info_data Android_API_17 ; extern const info_data Android_4_2              ;
		extern const info_data Android_API_18 ; extern const info_data Android_4_3              ;
		extern const info_data Android_API_19 ; extern const info_data Android_4_4              ; extern const info_data Android_KitKat            ;
		extern const info_data Android_API_20 ; extern const info_data Android_4_4W             ;
		extern const info_data Android_API_21 ; extern const info_data Android_5                ; extern const info_data Android_Lollipop          ;
		extern const info_data Android_API_22 ; extern const info_data Android_5_1              ;
		extern const info_data Android_API_23 ; extern const info_data Android_6                ; extern const info_data Android_Marshmallow       ;
		extern const info_data Android_API_24 ; extern const info_data Android_7                ; extern const info_data Android_Nougat            ;
		extern const info_data Android_API_25 ; extern const info_data Android_7_1              ;
		extern const info_data Android_API_26 ; extern const info_data Android_8                ; extern const info_data Android_Oreo              ;
		extern const info_data Android_API_27 ; extern const info_data Android_8_1              ;
		extern const info_data Android_API_28 ; extern const info_data Android_9                ; extern const info_data Android_Pie               ;
		extern const info_data Android_API_29 ; extern const info_data Android_10               ;
		extern const info_data Android_API_30 ; extern const info_data Android_11               ;
		extern const info_data Mac_OS_X_10_0  ;
		extern const info_data Mac_OS_X_10_1  ;
		extern const info_data Mac_OS_X_10_2  ; extern const info_data Mac_OS_X_Jaguar          ;
		extern const info_data Mac_OS_X_10_3  ; extern const info_data Mac_OS_X_Panther         ;
		extern const info_data Mac_OS_X_10_4  ; extern const info_data Mac_OS_X_Tiger           ;
		extern const info_data Mac_OS_X_10_5  ; extern const info_data Mac_OS_X_Leopard         ;
		extern const info_data Mac_OS_X_10_6  ; extern const info_data Mac_OS_X_Snow_Leopard    ;
		extern const info_data Mac_OS_X_10_7  ; extern const info_data Mac_OS_X_Lion            ;
		extern const info_data     OS_X_10_8  ; extern const info_data     OS_X_Mountain_Lion   ;
		extern const info_data     OS_X_10_9  ; extern const info_data     OS_X_Mavericks       ;
		extern const info_data     OS_X_10_10 ; extern const info_data     OS_X_Yosemite        ;
		extern const info_data     OS_X_10_11 ; extern const info_data     OS_X_El_Capitan      ;
		extern const info_data    macOS_10_12 ; extern const info_data    macOS_Sierra          ;
		extern const info_data    macOS_10_13 ; extern const info_data    macOS_High_Sierra     ;
		extern const info_data    macOS_10_14 ; extern const info_data    macOS_Mojave          ;
		extern const info_data    macOS_10_15 ; extern const info_data    macOS_Catalina        ;
		extern const info_data    macOS_11    ; extern const info_data    macOS_Big_Sur         ;
		extern const info_data iPhone_OS_1_0  ;
		extern const info_data iPhone_OS_1_1  ;
		extern const info_data iPhone_OS_2_0  ;
		extern const info_data iPhone_OS_2_1  ;
		extern const info_data iPhone_OS_2_2  ;
		extern const info_data iPhone_OS_3_0  ;
		extern const info_data iPhone_OS_3_1  ;
		extern const info_data iPhone_OS_3_2  ;
		extern const info_data       iOS_4_0  ;
		extern const info_data       iOS_4_1  ;
		extern const info_data       iOS_5_0  ;
		extern const info_data       iOS_5_1  ;
		extern const info_data       iOS_6_0  ;
		extern const info_data       iOS_6_1  ;
		extern const info_data       iOS_7_0  ;
		extern const info_data       iOS_7_1  ;
		extern const info_data       iOS_8_0  ;
		extern const info_data       iOS_8_1  ;
		extern const info_data       iOS_9_0  ;
		extern const info_data       iOS_9_1  ;
		extern const info_data       iOS_9_2  ;
		extern const info_data       iOS_9_3  ;
		extern const info_data       iOS_9_4  ;
		extern const info_data       iOS_10_0 ;
		extern const info_data       iOS_10_1 ;
		extern const info_data       iOS_10_2 ;
		extern const info_data       iOS_10_3 ;
		extern const info_data       iOS_11_0 ;
		extern const info_data       iOS_11_1 ;
		extern const info_data       iOS_11_2 ;
		extern const info_data       iOS_11_3 ;
		extern const info_data       iOS_12_0 ;
		extern const info_data       iOS_12_1 ;
		extern const info_data       iOS_12_2 ;
		extern const info_data       iOS_12_3 ;
		extern const info_data       iOS_12_4 ;
		extern const info_data       iOS_13_0 ;
		extern const info_data       iOS_13_1 ; extern const info_data iPadOS_13_1              ;
		extern const info_data       iOS_13_2 ; extern const info_data iPadOS_13_2              ;
		extern const info_data       iOS_13_3 ; extern const info_data iPadOS_13_3              ;
		extern const info_data       iOS_13_4 ; extern const info_data iPadOS_13_4              ;
		extern const info_data       iOS_13_5 ; extern const info_data iPadOS_13_5              ;
		extern const info_data       iOS_13_6 ; extern const info_data iPadOS_13_6              ;
		extern const info_data       iOS_13_7 ; extern const info_data iPadOS_13_7              ;
		extern const info_data       iOS_14_0 ; extern const info_data iPadOS_14_0              ;
		extern const info_data       iOS_14_1 ; extern const info_data iPadOS_14_1              ;
		extern const info_data       iOS_14_2 ; extern const info_data iPadOS_14_2              ;
		extern const info_data       iOS_14_3 ; extern const info_data iPadOS_14_3              ;
	}
};

bool operator==(const ljh::system_info::platform&, const ljh::system_info::info_data&);
bool operator==(const ljh::version              &, const ljh::system_info::info_data&);
bool operator!=(const ljh::version              &, const ljh::system_info::info_data&);
bool operator< (const ljh::version              &, const ljh::system_info::info_data&);
bool operator> (const ljh::version              &, const ljh::system_info::info_data&);
bool operator>=(const ljh::version              &, const ljh::system_info::info_data&);
bool operator<=(const ljh::version              &, const ljh::system_info::info_data&);
bool operator==(const ljh::u32                  &, const ljh::system_info::info_data&);
bool operator!=(const ljh::u32                  &, const ljh::system_info::info_data&);
bool operator< (const ljh::u32                  &, const ljh::system_info::info_data&);
bool operator> (const ljh::u32                  &, const ljh::system_info::info_data&);
bool operator>=(const ljh::u32                  &, const ljh::system_info::info_data&);
bool operator<=(const ljh::u32                  &, const ljh::system_info::info_data&);