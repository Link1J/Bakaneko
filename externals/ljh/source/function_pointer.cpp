
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ljh/function_pointer.hpp"
#include "ljh/system_info.hpp"

#if defined(LJH_TARGET_Windows)
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <windows.h>
#    define GET_ERROR() GetLastError()
#else
#    include <dlfcn.h>
#    define GET_ERROR() 0
#endif

namespace ljh
{
	bool _load_function(void* loaded_dll, const char* function_name, void** function, u32* error);
}

bool ljh::_load_dll(const char* dll_name, const char* function_name, void** function, u32* error)
{
	*function = nullptr;
#if defined(LJH_TARGET_Windows)
	auto loaded_dll = LoadLibraryA(dll_name);
#else
	auto loaded_dll = dlopen(dll_name, RTLD_LAZY);
#endif
	if (loaded_dll == nullptr)
	{
		*error = GET_ERROR();
		return false;
	}
	return _load_function(loaded_dll, function_name, function, error);
}

bool ljh::_load_function(void* loaded_dll, const char* function_name, void** function, u32* error)
{
#if defined(LJH_TARGET_Windows)
	*function = GetProcAddress((HMODULE)loaded_dll, function_name);
#else
	*function = dlsym(loaded_dll, function_name);
#endif
	if (*function == nullptr)
	{
		*error = GET_ERROR();
		return false;
	}
	return true;
}