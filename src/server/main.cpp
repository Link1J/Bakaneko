// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "rest.hpp"
#include "info.hpp"

#include <ljh/system_info.hpp>

#if defined(LJH_TARGET_Windows)
#include <ljh/windows/wmi.hpp>
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#if defined(LJH_TARGET_Windows)
#include "spdlog/sinks/win_eventlog_sink.h"
#elif defined(LJH_TARGET_Linux)
#include "spdlog/sinks/syslog_sink.h"
#endif

int main(int argc, const char* argv[])
{
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#if defined(LJH_TARGET_Windows)
    sinks.push_back(std::make_shared<spdlog::sinks::win_eventlog_sink_st>("bakaneko-server"));
#elif defined(LJH_TARGET_Linux)
    //sinks.push_back(std::make_shared<spdlog::sinks::syslog_sink_mt>("bakaneko-server", LOG_PID, LOG_USER, false));
#endif
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("", std::begin(sinks), std::end(sinks)));
    spdlog::register_logger(std::make_shared<spdlog::logger>("networking", std::begin(sinks), std::end(sinks)));

    spdlog::info("Starting Bakaneko Server (Version {})", BAKANEKO_VERSION_STRING);

#if defined(LJH_TARGET_Windows)
    winrt::init_apartment();
    ljh::windows::wmi::setup();
#endif

    int const thread_count = std::thread::hardware_concurrency();
    asio::io_context io_service{thread_count};

    auto const address = asio::ip::make_address("0.0.0.0");
    auto const port    = (unsigned short)std::atoi("8080");

    std::make_shared<Rest::Server>(io_service, asio::ip::tcp::endpoint{address, port})->run();

    std::vector<std::thread> thread;
    thread.reserve(thread_count - 1);
    for(auto i = thread_count - 1; i > 0; --i)
        thread.emplace_back([&io_service]{ io_service.run(); });
    io_service.run();
    
    spdlog::info("Stoping Bakaneko Server");

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