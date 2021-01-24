// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once
#include <cstdlib>
#include <type_traits>
#include <ljh/system_info.hpp>
#include <ljh/function_traits.hpp>

#if defined(LJH_TARGET_Windows)
template<typename T>
std::remove_pointer_t<T>* array_malloc(std::size_t size)
{
    return (decltype(array_malloc<T>(0)))malloc(size * sizeof(std::remove_pointer_t<T>));
}

template<typename T>
std::shared_ptr<std::remove_pointer_t<T>[]> shared_ptr_array(std::size_t size)
{
    return decltype(shared_ptr_array<T>(0)){array_malloc<T>(size)};
}

template<typename F, typename... A, typename = std::enable_if_t<std::is_invocable_v<F, A..., std::nullptr_t, std::nullptr_t>>>
auto Win32Run(F&& function, A&&... args)
{
    using N = ljh::function_traits<std::remove_reference_t<F>>;
    using I = typename std::tuple_element<N::argument_count - 1, N::argument_types>::type;
    using P = typename std::tuple_element<N::argument_count - 2, N::argument_types>::type;
    std::remove_pointer_t<I> size = 0;
    function(std::forward<A>(args)..., nullptr, &size);
    auto data = [&size] {
        if constexpr (std::is_same_v<P, char*>)
            return std::string(size, '\0');
        else if constexpr (std::is_same_v<P, wchar_t*>)
            return std::wstring(size, '\0');
        else
            return shared_ptr_array<P>(size);
    }();
    function(std::forward<A>(args)..., &data[0], &size);
    return data;
}
#endif