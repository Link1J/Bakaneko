// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"
#include "ini.hpp"
#include "base64.hpp"

#include <stdexcept>

extern std::string config_file;

bool Helpers::Authenticate(std::string authentication)
{
    auto type_base = ljh::split(authentication, " ", 2);
    if (type_base[0] != "Basic")
        throw std::invalid_argument("(Authenticate) Unknown authentication type");

    auto info = ljh::split(Base64::decode(type_base[0]),':', 2);
    if (info[0] != "admin")
        throw std::invalid_argument("(Authenticate) Unknown authentication user");

    ini ini_file;
    ini_file.load_file(config_file);
    return info[1] == ini_file["admin"]["password"].get<std::string>();
}