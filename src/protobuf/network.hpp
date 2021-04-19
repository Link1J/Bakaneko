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

    struct Adapter
    {
        enum State
        {
            Down = 0,
            Up = 1,
        };

        std::string name;
        State state;
        uint64_t link_speed;
        uint64_t mtu;
        std::string mac_address;
        std::string ip_address;
        uint64_t bytes_rx; // Total Bytes received
        uint64_t bytes_tx; // Total Bytes sent
        uint64_t time;     // Time the data bytes_rx and bytes_tx were gotten
    };

    struct Adapters
    {
        std::vector<Adapter> adapters;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Adapter, name, state, link_speed, mtu, mac_address, ip_address, bytes_rx, bytes_tx, time)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Adapters, adapters)
}