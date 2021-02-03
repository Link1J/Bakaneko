// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once
#include <cstdlib>
#include <type_traits>
#include <ljh/system_info.hpp>
#include <ljh/function_traits.hpp>

#if defined(LJH_TARGET_Windows)
#include <windows.h>

template<typename T>
std::shared_ptr<std::remove_pointer_t<T>> malloc_shared_ptr(std::size_t size)
{
    return decltype(malloc_shared_ptr<T>(0)){(std::remove_pointer_t<T>*)malloc(size)};
}

template<typename F, typename... A>
constexpr auto _Win32Run_PI()
{
    if constexpr (std::is_invocable_v<F, A..., std::nullptr_t, std::nullptr_t>)
        return (std::size_t)sizeof...(A);
    else if constexpr (std::is_invocable_v<F, std::nullptr_t, std::nullptr_t, A...>)
        return (std::size_t)0;
    else if constexpr (std::is_invocable_v<F, A..., std::nullptr_t>)
        return (std::size_t)sizeof...(A);
    else if constexpr (std::is_invocable_v<F, std::nullptr_t, A...>)
        return (std::size_t)0;
    else
        return (std::size_t)-1;
}

template<typename F, typename... A>
constexpr auto _Win32Run_II()
{
    return _Win32Run_PI<F,A...>() + 1;
}

template<typename F>
using _Win32Run_T = typename ljh::function_traits<std::remove_reference_t<F>>::argument_types;

template<typename F, typename... A>
using _Win32Run_PT = typename std::tuple_element<_Win32Run_PI<F,A...>(), _Win32Run_T<F>>::type;

template<typename F, typename... A>
using _Win32Run_IT = typename std::tuple_element<_Win32Run_II<F,A...>(), _Win32Run_T<F>>::type;

template<typename F, typename... A>
auto _Win32Run_RT(std::remove_pointer_t<_Win32Run_IT<F,A...>> size)
{
    if constexpr (std::is_same_v<_Win32Run_PT<F,A...>, char*>)
        return std::string(size, '\0');
    else if constexpr (std::is_same_v<_Win32Run_PT<F,A...>, wchar_t*>)
        return std::wstring(size, '\0');
    else
        return malloc_shared_ptr<_Win32Run_PT<F,A...>>(size);
}

namespace std
{
    template <class C>
    constexpr auto data(C& c) -> decltype(c.get())
    {
        return c.get();
    }
}

template<typename F>
std::enable_if_t<std::is_invocable_v<F, std::nullptr_t, std::nullptr_t>, decltype(_Win32Run_RT<F>(0))>
Win32Run(F&& function)
{
    std::remove_pointer_t<_Win32Run_IT<F>> size = 0;
    function(nullptr, &size);
    auto data = _Win32Run_RT<F>(size);
    function(std::data(data), &size);
    if constexpr (ljh::is_instance_v<decltype(data), std::basic_string>)
        data = data.substr(0, data.size() - 1);
    return data;
}

template<typename F, typename... A>
std::enable_if_t<std::is_invocable_v<F, A..., std::nullptr_t, std::nullptr_t>, decltype(_Win32Run_RT<F,A...>(0))>
Win32Run(F&& function, A&&... args)
{
    std::remove_pointer_t<_Win32Run_IT<F,A...>> size = 0;
    function(std::forward<A>(args)..., nullptr, &size);
    auto data = _Win32Run_RT<F,A...>(size);
    function(std::forward<A>(args)..., std::data(data), &size);
    if constexpr (ljh::is_instance_v<decltype(data), std::basic_string>)
        data = data.substr(0, data.size() - 1);
    return data;
}

template<typename F, typename... A>
std::enable_if_t<std::is_invocable_v<F, std::nullptr_t, std::nullptr_t, A...>, decltype(_Win32Run_RT<F,A...>(0))>
Win32Run(F&& function, A&&... args)
{
    std::remove_pointer_t<_Win32Run_IT<F,A...>> size = 0;
    function(nullptr, &size, std::forward<A>(args)...);
    auto data = _Win32Run_RT<F,A...>(size);
    function(std::data(data), &size, std::forward<A>(args)...);
    if constexpr (ljh::is_instance_v<decltype(data), std::basic_string>)
        data = data.substr(0, data.size() - 1);
    return data;
}

template<typename D, typename F, typename... A>
std::enable_if_t<std::is_invocable_v<F, A..., std::nullptr_t>, std::shared_ptr<std::remove_pointer_t<std::remove_pointer_t<_Win32Run_PT<F, A...>>>>>
Win32Run(D&& deleter, F&& function, A&&... args)
{
    std::remove_pointer_t<_Win32Run_PT<F, A...>> data;
    function(std::forward<A>(args)..., &data);
    return std::shared_ptr<std::remove_pointer_t<decltype(data)>>(data, std::forward<D>(deleter));
}

template<typename D, typename F, typename... A>
std::enable_if_t<std::is_invocable_v<F, std::nullptr_t, A...>, std::shared_ptr<std::remove_pointer_t<std::remove_pointer_t<_Win32Run_PT<F, A...>>>>>
Win32Run(D&& deleter, F&& function, A&&... args)
{
    std::remove_pointer_t<_Win32Run_PT<F, A...>> data;
    function(&data, std::forward<A>(args)...);
    return std::shared_ptr<std::remove_pointer_t<decltype(data)>>(data, std::forward<D>(deleter));
}

template<typename HandleType = HANDLE, BOOL(__stdcall* Close)(HandleType) = CloseHandle>
class Win32Handle
{
    HandleType handle;

public:
    template<typename F, typename... A, typename = std::enable_if<std::is_invocable_v<F, A...>>>
    explicit Win32Handle(F&& function, A&&... args)
    {
        handle = function(std::forward<A>(args)...);
    }

    ~Win32Handle()
    {
        Close(handle);
    }

    Win32Handle(const Win32Handle& other)
    {
        DuplicateHandle(GetCurrentProcess(), other, GetCurrentProcess(), &handle, 0, FALSE, DUPLICATE_SAME_ACCESS);
        return *this;
    }

    Win32Handle& operator=(const Win32Handle& other)
    {
        DuplicateHandle(GetCurrentProcess(), other, GetCurrentProcess(), &handle, 0, FALSE, DUPLICATE_SAME_ACCESS);
        return *this;
    }

    operator HandleType() const
    {
        return handle;
    }

    bool operator==(const Win32Handle& other) const
    {
        return CompareObjectHandles(handle, other);
    }

    bool operator!=(const Win32Handle& other) const
    {
        return !(*this == other);
    }
};

using Win32ServiceHandle = Win32Handle<SC_HANDLE, CloseServiceHandle>;

#endif