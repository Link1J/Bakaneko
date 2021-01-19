
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// console.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires Windows SDK
// 
// ABOUT
//     Sets the Windows Console codepage to UTF-8
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#include <windows.h>
#include <stdio.h>

namespace ljh 
{
	namespace windows 
	{
		namespace console 
		{
			void make_UTF8()
			{
				SetConsoleOutputCP(CP_UTF8);
			}
			
			void attach_streams()
			{
				if (GetStdHandle(STD_OUTPUT_HANDLE))
				{
					freopen("CONOUT$", "w", stdout);
					setvbuf(stdout, NULL, _IONBF, 0);
				}
				if (GetStdHandle(STD_INPUT_HANDLE ))
				{
					freopen("CONIN$", "r", stdin);
					setvbuf(stdin, NULL, _IONBF, 0);
				}
				if (GetStdHandle(STD_ERROR_HANDLE))
				{
					freopen("CONOUT$", "w", stderr);
					setvbuf(stderr, NULL, _IONBF, 0);
				}
			}

			void attach_to_parent()
			{
				AttachConsole(ATTACH_PARENT_PROCESS);
				attach_streams();
			}
			
			void create()
			{
				AllocConsole();
				attach_streams();
			}
		}
	}
}