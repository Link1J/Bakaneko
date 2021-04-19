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

    struct ServiceInfo
    {
        std::string server;

        std::vector<std::string> types;
    };

    struct ServicesRequest
    {
        std::string type;
    };

    struct Service
    {
        enum State
        {
            Stopped = 0,
            Running = 1,
            Starting = 2,
            Stopping = 3,
        };

        std::string id;
        State state;
        bool enabled;
        std::string type;

        // Display data
        std::string display_name;
        std::string description;

        struct Control
        {
            enum Action
            {
                Stop = 0,
                Start = 1,
                Restart = 2,
                Enable = 3,
                Disable = 4,
            };

            std::string id;
            Action action;
        };
    };

    struct Services
    {
        std::vector<Service> services;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Service, id, state, enabled, type, display_name, description)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServiceInfo, server, types)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Services, services)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServicesRequest, type)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Service::Control, id, action)
}