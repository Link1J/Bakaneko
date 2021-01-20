
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// os_build_info.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++
// 
// ABOUT
//     What OS is current being used for builds
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    define LJH_TARGET_Windows
#elif __APPLE__
#    include <TargetConditionals.h>
#    if TARGET_OS_IPHONE
#        define LJH_TARGET_iOS
#    elif TARGET_OS_MAC
#        define LJH_TARGET_MacOS
#    endif
#elif __ANDROID__
#    define LJH_TARGET_Android
#elif __linux__
#    define LJH_TARGET_Linux
#elif __unix__
#    define LJH_TARGET_Unix
#endif