
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// system_directories.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++
// 
// ABOUT
//     Functions for getting system directories.
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#include <string>

namespace ljh
{
	namespace system_directories
	{
		std::string home      ();
		std::string cache     ();
		std::string config    ();
		std::string data      ();
		std::string documents ();
		std::string desktop   ();
		std::string pictures  ();
		std::string music     ();
		std::string videos    ();
		std::string downloads ();
		std::string save_games();
	}
}