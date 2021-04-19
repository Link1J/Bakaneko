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
    struct Partition
    {
        std::string dev_node;
        uint64_t size;
        uint64_t used;
        std::string mountpoint;
        std::string filesystem;
    };

    struct Drive
    {
        std::string dev_node;
        uint64_t size;
        std::string model;
        std::string manufacturer;
        std::string interface;
        std::vector<Partition> partitions;
    };

    struct Drives
    {
        std::vector<Drive> drives;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Partition, dev_node, size, used, mountpoint, filesystem)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Drive, dev_node, size, model, manufacturer, interface, partitions)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Drives, drives)
}