// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "rest.hpp"
#include "info.hpp"

#include <ljh/system_info.hpp>

#if defined(LJH_TARGET_Windows)
#include <ljh/windows/wmi.hpp>
#endif

#include "windows.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#if defined(LJH_TARGET_Windows)
#include "spdlog/sinks/win_eventlog_sink.h"
#elif defined(LJH_TARGET_Linux)
#include "spdlog/sinks/syslog_sink.h"
#endif

#include <charconv>

constexpr auto DEFAULT_ADDRESS = "0.0.0.0";
constexpr auto DEFAULT_PORT    =   "29921"; // Unless a well know service uses this port. Do not change it.

int const thread_count = std::thread::hardware_concurrency();
asio::io_context io_service{thread_count};

namespace std
{
    int atoi(const string_view& view) noexcept
    {
        int i3 = 0;
        std::from_chars(view.data(), view.data() + view.size(), i3);
        return i3;
    }
}

#if defined(LJH_TARGET_Windows)
extern "C" int real_main(int argc, const char* argv[])
#else
int main(int argc, const char* argv[])
#endif
{
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#if defined(LJH_TARGET_Windows)
    sinks.push_back(std::make_shared<spdlog::sinks::win_eventlog_sink_st>("Bakaneko Server"));
#elif defined(LJH_TARGET_Linux)
    //sinks.push_back(std::make_shared<spdlog::sinks::syslog_sink_mt>("bakaneko-server", LOG_PID, LOG_USER, false));
#endif
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("", std::begin(sinks), std::end(sinks)));
    spdlog::register_logger(std::make_shared<spdlog::logger>("networking", std::begin(sinks), std::end(sinks)));

    std::string_view address_string = DEFAULT_ADDRESS;
    std::string_view port_string    = DEFAULT_PORT   ;

    for (int a = 0; a < argc; a++)
    {
        using namespace std::literals;
        if (argv[a] == "--help"sv || argv[a] == "-h"sv)
        {
            printf("\n Usage: %s [OPTIONS]\n\n", argv[0]);
            printf("  Options:\n");
            printf("   -h --help                 Print this help\n");
            printf("      --address address      Address to listen on\n");
            printf("      --port    port         Port to listen on\n");
            printf("      --install              Installs the Windows Service (needs Admin)\n");
            printf("\n");
            exit(0);
        }
        else if (argv[a] == "--address"sv)
        {
            a++;
            address_string = argv[a];
        }
        else if (argv[a] == "--port"sv)
        {
            a++;
            port_string = argv[a];
        }
    }

    spdlog::info("Starting Bakaneko Server (Version {})", BAKANEKO_VERSION_STRING);

    try
    {
#if defined(LJH_TARGET_Windows)
        winrt::init_apartment();
        ljh::windows::wmi::setup();

        try
        {
            HANDLE hToken;
            TOKEN_PRIVILEGES tkp;
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
                throw 1;
            LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
            if (GetLastError() != ERROR_SUCCESS)
                throw 1;
        }
        catch (int)
        {
            spdlog::warn("Failed to get shutdown privilege. Power controls may not work.");
        }
#endif

        auto const address = asio::ip::make_address(address_string);
        auto const port    = (unsigned short)std::atoi(port_string);

        std::make_shared<Rest::Server>(io_service, asio::ip::tcp::endpoint{address, port})->run();

        std::vector<std::thread> thread;
        thread.reserve(thread_count - 1);
        for(auto i = thread_count - 1; i > 0; --i)
            thread.emplace_back([]{ io_service.run(); });
        io_service.run();
        for(auto& thr : thread)
            if (thr.joinable())
                thr.join();
    }
    catch(const std::exception& e)
    {
        spdlog::error(e.what());
    }

    spdlog::info("Stopping Bakaneko Server");

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
#include <fstream>

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
    try
    {
        ljh::memory_mapped::view view{file, ljh::memory_mapped::permissions::read, 0, file.size()};
        return std::string{view.as<const char>(), file.size()};
    }
    catch (const ljh::memory_mapped::invalid_file& e)
    {
        std::ifstream in(file_path, std::ios::in | std::ios::binary);
        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(std::data(contents), contents.size());
            in.close();
            
            if (auto null_term = contents.find('\0'); null_term == 0)
                return "";
            else if (null_term != std::string::npos)
                return contents.substr(0, null_term - 1);
            return contents;
        }
        throw e;
    }
}