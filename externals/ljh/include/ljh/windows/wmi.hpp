
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// wmi.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// Requires winrt
// 
// ABOUT
//     A wrapper for WMI
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#define NOMINMAX
#define _WIN32_DCOM
#include <OAIdl.h>
#include <wbemidl.h>
#include <winrt/base.h>
#include <ljh/windows/com_bstr.hpp>
#include <ljh/windows/com_variant.hpp>

namespace ljh::windows::wmi
{
    class clazz
    {
        friend class service;
        winrt::com_ptr<IWbemServices> _i_service;
        winrt::com_ptr<IWbemClassObject> _i_object;

        com_variant _i_get(const std::wstring& member);

    public:
        clazz(const winrt::com_ptr<IWbemServices>& _i_service, const winrt::com_ptr<IWbemClassObject>& _i_object);

        template<typename T = std::wstring>
        T get(const std::wstring& member)
        {
            return _i_get(member).as<T>();
        }

        bool has(const std::wstring& member);

        std::vector<std::wstring> member_names();

        std::vector<clazz> associators();
        std::vector<clazz> associators(const std::wstring& assoc_class);
    };

    class service
    {
        winrt::com_ptr<IWbemServices> _i_service;

    public:
        service(const com_bstr& location);

        std::vector<clazz> get_class(const std::wstring& class_name);
    };

    void setup();
}