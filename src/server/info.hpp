// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once
#include <ljh/expected.hpp>

#include "server.pb.h"
#include "updates.pb.h"
#include "drives.pb.h"
#include "network.pb.h"

enum class Errors
{
    None, NotImplemented, Failed
};

namespace Info
{
    ljh::expected<Bakaneko::Drives  , Errors> Drives  ();
    ljh::expected<Bakaneko::Updates , Errors> Updates ();
    ljh::expected<Bakaneko::System  , Errors> System  ();
    ljh::expected<Bakaneko::Adapters, Errors> Adapters();
};

namespace Control
{
    ljh::expected<void, Errors> Shutdown();
    ljh::expected<void, Errors> Reboot  ();
};