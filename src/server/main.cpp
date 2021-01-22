// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "rest.hpp"
#include "info.hpp"

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

    server.add_handler("/", [](const Http::Request&) -> Http::Response {
        Http::Response response(302);
        response.set_content("text/plain", "Available");
        return response;
    });

    server.add_handler("/drives", &Info::Drives );
    server.add_handler("/update", &Info::Updates);
    server.add_handler("/system", &Info::System );

    server.start_listening();
    io_service.run();
    return 0;
}

// Don't know a better place for the code below

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <filesystem>
#include <ljh/memory_mapped_file.hpp>

#if defined(LJH_TARGET_Windows)
#define popen _popen
#define pclose _pclose
#define re_stderr "NUL"
#else
#define re_stderr "/dev/null"
#endif

std::tuple<int, std::string> exec(const std::string& cmd)
{
    auto pipe = popen((cmd + " 2>" re_stderr).c_str(), "r");
    if (!pipe)
        throw std::runtime_error("popen() failed!");

    std::array<char, 128> buffer;
    std::string result;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
        result += buffer.data();

    return {pclose(pipe), result};
}

std::string read_file(std::filesystem::path file_path)
{
    ljh::memory_mapped::file file(std::forward<decltype(file_path)>(file_path), ljh::memory_mapped::permissions::read);
    ljh::memory_mapped::view view{file, ljh::memory_mapped::permissions::read, 0, file.size()};
    return std::string{view.as<const char>(), file.size()};
}