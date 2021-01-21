// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "rest.hpp"
#include "drives.hpp"

#include <ljh/system_info.hpp>

#if defined(LJH_TARGET_Windows)
#include <ljh/windows/wmi.hpp>
#endif

int main(int argc, const char* argv[])
{
#if defined(LJH_TARGET_Windows)
    winrt::init_apartment();
    ljh::windows::wmi::setup();
#endif

    asio::io_service io_service;
    asio::ip::tcp::endpoint endpoint{asio::ip::tcp::v4(), 8080};
    Rest::Server server{io_service, endpoint};

    server.add_handler("/drives", &Info::Drives);

    server.start_listening();
    io_service.run();
    return 0;
}