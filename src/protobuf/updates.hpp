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
    struct Update
    {
        std::string source;
        std::string name;
        std::string old_version;
        std::string new_version;
    };

    struct Updates
    {
        std::vector<Update> updates;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Update, source, name, old_version, new_version)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Updates, updates)
}