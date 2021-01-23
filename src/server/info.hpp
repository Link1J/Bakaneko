// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once
#include <ljh/expected.hpp>

#include "server.pb.h"
#include "updates.pb.h"
#include "drives.pb.h"

enum class Errors
{
    None, NotImplemented,
};

namespace Info
{
    ljh::expected<Bakaneko::Drives , Errors> Drives ();
    ljh::expected<Bakaneko::Updates, Errors> Updates();
    ljh::expected<Bakaneko::System , Errors> System ();
};