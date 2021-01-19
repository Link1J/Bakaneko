
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// get_index.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++
// Requires cpp_version.hpp
// 
// ABOUT
//     Get index of element
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#include "cpp_version.hpp"
#include <algorithm>

namespace ljh
{
	template<typename It, typename T>
	LJH_CPP20_CONSTEXPR auto get_index(It _First, const It _Last, const T& _Val)
	{
		return std::distance(_First, std::find(_First, _Last, _Val));
	}

	template<typename It, typename Pr>
	LJH_CPP20_CONSTEXPR auto get_index_if(It _First, const It _Last, Pr _Pre)
	{
		return std::distance(_First, std::find_if(_First, _Last, _Pre));
	}
}