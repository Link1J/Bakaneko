// SPDX-License-Identifier: GPL-3.0-or-later OR BSL-1.0
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

/*
    This file is licensed under GPL v3 or later, and Boost Software Licence.
    This is so that you may use this file in your own projects without needing
    to follow the GPL license.
*/

#pragma once

#include <nlohmann/json.hpp>

#include <cstdint>
#include <vector>
#include <string>

namespace Bakaneko
{
    struct System
    {
        std::string hostname;
        std::string mac_address;
        std::string ip_address;
        std::string operating_system;
        std::string kernel;
        std::string architecture;
        std::string vm_platform;
        std::string icon;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(System, hostname, mac_address, ip_address, operating_system, kernel, architecture, vm_platform, icon)
}