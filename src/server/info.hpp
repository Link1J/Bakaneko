// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once
#include <ljh/expected.hpp>
#include <optional>
#include <string>

#include "server.hpp"
#include "updates.hpp"
#include "drives.hpp"
#include "network.hpp"
#include "services.hpp"

enum class Errors
{
    None, NotImplemented, Failed, NeedsPassword,
};

struct Fields
{
    std::optional<std::string> authentication;
};

namespace Helpers
{
    bool Authenticate(std::string authentication);
}

namespace Info
{
    ljh::expected<Bakaneko::Drives     , Errors> Drives  (const Fields& fields);
    ljh::expected<Bakaneko::Updates    , Errors> Updates (const Fields& fields);
    ljh::expected<Bakaneko::System     , Errors> System  (const Fields& fields);
    ljh::expected<Bakaneko::Adapters   , Errors> Adapters(const Fields& fields);
    ljh::expected<Bakaneko::ServiceInfo, Errors> Service (const Fields& fields);
    ljh::expected<Bakaneko::Services   , Errors> Services(const Fields& fields, Bakaneko::ServicesRequest data);
};

namespace Control
{
    ljh::expected<void, Errors> Shutdown(const Fields& fields);
    ljh::expected<void, Errors> Reboot  (const Fields& fields);
    ljh::expected<void, Errors> Service (const Fields& fields, Bakaneko::Service::Control data);
};