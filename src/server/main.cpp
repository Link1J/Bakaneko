// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "rest.hpp"
#include "info.hpp"

#include <ljh/system_info.hpp>

#if defined(LJH_TARGET_Windows)
#include <ljh/windows/wmi.hpp>
#endif

#include "windows.hpp"
#include "ini.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#if defined(LJH_TARGET_Windows)
#include "spdlog/sinks/win_eventlog_sink.h"
#elif defined(LJH_TARGET_Linux)
#include "spdlog/sinks/syslog_sink.h"
#endif

#include <charconv>

constexpr auto DEFAULT_ADDRESS     = "0.0.0.0";
constexpr auto DEFAULT_PORT        =     29921;
constexpr auto DEFAULT_CONFIG_FILE = []() constexpr {
#if defined(LJH_TARGET_Windows)
    return "bakaneko-server.ini";
#elif defined(LJH_TARGET_MacOS)
    throw "/etc/bakaneko-server.ini";
#elif defined(LJH_TARGET_Linux)
    return "/etc/bakaneko-server.ini";
#elif defined(LJH_TARGET_Unix)
    return "/etc/bakaneko-server.ini";
#endif
}();

int const thread_count = std::thread::hardware_concurrency();
asio::io_context io_service{thread_count};
std::string config_file;

class cstring
{
    const char* string;
public:
    cstring(const char* string)
        : string(string)
    {}

    bool operator==(const std::string_view& other)
    {
        return string == other;
    }

    operator std::string()
    {
        return string;
    }

    explicit operator const char*()
    {
        return string;
    }

    std::size_t size() const
    {
        return strlen(string);
    }
    
    const char*  begin()       { return  string        ; }
    const char* cbegin() const { return  string        ; }
    const char*  begin() const { return  string        ; }
    const char*  end  ()       { return &string[size()]; }
    const char* cend  () const { return &string[size()]; }
    const char*  end  () const { return &string[size()]; }
};

template<typename T>
class carray
{
    T* array;
    std::size_t _size;
public:
    carray(void* array, std::size_t size)
        : array((T*)array), _size(size)
    {}

    T& operator[](std::size_t idx)
    {
        return array[idx];
    }

    const T& operator[](std::size_t idx) const
    {
        return array[idx];
    }

    std::size_t size() const
    {
        return _size;
    }

          T*  begin()       { return  array        ; }
    const T* cbegin() const { return  array        ; }
    const T*  begin() const { return  array        ; }
          T*  end  ()       { return &array[size()]; }
    const T* cend  () const { return &array[size()]; }
    const T*  end  () const { return &array[size()]; }
};

namespace std
{
    int atoi(const string_view& view) noexcept
    {
        int i3 = 0;
        std::from_chars(view.data(), view.data() + view.size(), i3);
        return i3;
    }

    int atoi(const cstring& view) noexcept
    {
        int i3 = 0;
        std::from_chars(view.begin(), view.end(), i3);
        return i3;
    }
}

struct ParsedArgs {
    std::optional<std::string> address    ;
    std::optional<uint16_t   > port       ;
    std::optional<std::string> config_file;
};
ParsedArgs parse_args(carray<cstring> args)
{
    auto print_help = [&args](int exit_code){
        printf("\nUsage: %s [OPTIONS]\n\n", (const char*)args[0]);
        printf("  Options:\n");
        printf("    -h --help                 Print this help\n");
        printf("       --address address      Address to listen on\n");
        printf("       --port    port         Port to listen on\n");
        printf("       --install              Installs the Windows Service (needs Admin)\n");
        printf("    -c --config  filename     Config file to use\n");
        printf("\n");
        exit(exit_code);
    };

    decltype(parse_args(std::declval<carray<cstring>>())) return_value;

    for (auto it = args.begin() + 1; it != args.end(); it++)
    {
        auto& arg = *it;
        using namespace std::literals;
        if (arg == "--help" || arg == "-h")
        {
            print_help(0);
        }
        else if (arg == "--address")
        {
            arg = *++it;
            return_value.address = arg;
        }
        else if (arg == "--port")
        {
            arg = *++it;
            return_value.port = boost::lexical_cast<uint16_t, std::string>(arg);
        }
        else if (arg == "--config" || arg == "-c")
        {
            arg = *++it;
            return_value.config_file = arg;
        }
        else
        {
            printf("Unknown arg: %s\n", (const char*)arg);
            print_help(-1);
        }
    }

    return return_value;
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

    spdlog::set_level(spdlog::level::debug);

    try
    {
        auto args = parse_args(carray<cstring>(argv, argc));

        config_file = args.config_file.value_or(DEFAULT_CONFIG_FILE);

        spdlog::info("Starting Bakaneko Server (Version {})", BAKANEKO_VERSION_STRING);
        spdlog::info("Using config file '{}'", config_file);
        
        auto [address, port] = [&args]{
            ini ini_file;
            ini_file.load_file(config_file);
            if (!ini_file["admin"].has("password"))
            {
                spdlog::error("Config file does not have a password under the admin section. Please add one.");
                exit(-2);
            }

            auto& networking = ini_file["networking"];
            return std::make_tuple(
                args.address.value_or(ini_file["networking"]["address"].get<std::string  >(DEFAULT_ADDRESS)),
                args.port   .value_or(ini_file["networking"]["port"   ].get<std::uint16_t>(DEFAULT_PORT   ))
            );
        }();

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
        std::make_shared<Rest::Server>(io_service, asio::ip::tcp::endpoint{asio::ip::make_address(address), port})->run();

        std::vector<std::thread> thread;
        thread.reserve(thread_count - 1);
        for(auto i = thread_count - 1; i > 0; --i)
            thread.emplace_back([]{ io_service.run(); });
        io_service.run();
        for(auto& thr : thread)
            if (thr.joinable())
                thr.join();

        spdlog::info("Stopping Bakaneko Server");
    }
    catch(const std::exception& e)
    {
        spdlog::error(e.what());
    }

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