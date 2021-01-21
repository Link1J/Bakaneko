// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once
#include "REST.hpp"

namespace Info
{
    Http::Response Drives(const Http::Request& request);
};