// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"

#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <string>

#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>
#include <ljh/casting.hpp>

#if defined(LJH_TARGET_Windows)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <Mstcpip.h>
#include <Ip2string.h>
#include "windows.hpp"
#include <ljh/windows/wmi.hpp>
#include <ljh/windows/registry.hpp>
#elif defined(LJH_TARGET_Linux)
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/reboot.h>
#include <unistd.h>
#include <ifaddrs.h>
#endif

#undef interface

inline std::string chassis_type_as_system_icon(int a);
extern std::string read_file(std::filesystem::path file_path);

std::string remove_quotes(std::string input)
{
    if (input.front() == '"' && input.back() == '"')
        input = input.substr(1, input.size() - 2);
    return input;
}

ljh::expected<Bakaneko::System, Errors> Info::System(const Fields &fields)
{
    decltype(Info::System(fields))::value_type system;

    char ip_address[47];
    memset(ip_address, 0, sizeof(ip_address));

    char mac_address[6 * 3 + 1];
    memset(mac_address, 0, sizeof(mac_address));

#if defined(LJH_TARGET_Windows)
    using namespace std::string_literals;

    static auto enclosure = ljh::windows::wmi::service::root().get_class(L"Win32_SystemEnclosure")[0];
    int chassis_type = enclosure.get<ljh::windows::com_safe_array<int32_t>>(L"ChassisTypes")[0];

    auto ip_addresses = Win32Run(GetAdaptersAddresses, AF_UNSPEC, 0, nullptr);
    for (auto adapter = ip_addresses.get(); adapter != nullptr; adapter = adapter->Next)
    {
        for (auto address = adapter->FirstUnicastAddress; address != nullptr; address = address->Next)
        {
            if (address->Address.lpSockaddr->sa_family == AF_INET)
            {
                RtlIpv4AddressToStringA(&ljh::pointer_cast<struct sockaddr_in>(address->Address.lpSockaddr)->sin_addr, ip_address);
                goto GOT_ADDRESS;
            }
        }
        for (auto address = adapter->FirstUnicastAddress; address != nullptr; address = address->Next)
        {
            if (address->Address.lpSockaddr->sa_family == AF_INET6)
            {
                RtlIpv6AddressToStringA(&ljh::pointer_cast<struct sockaddr_in6>(address->Address.lpSockaddr)->sin6_addr, ip_address);
                goto GOT_ADDRESS;
            }
        }
        continue;
    GOT_ADDRESS:
        sprintf(mac_address, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
                adapter->PhysicalAddress[0], adapter->PhysicalAddress[1], adapter->PhysicalAddress[2],
                adapter->PhysicalAddress[3], adapter->PhysicalAddress[4], adapter->PhysicalAddress[5]);
        break;
    }

    system.hostname = (Win32Run(GetComputerNameExA, ComputerNamePhysicalDnsHostname));
    system.operating_system = (*ljh::system_info::get_string());
    system.kernel = ("NT "s + std::string{*ljh::system_info::get_version()});
    system.architecture = (std::getenv("PROCESSOR_ARCHITECTURE"));
    system.icon = (chassis_type_as_system_icon(chassis_type));
#elif defined(LJH_TARGET_Linux)
    struct utsname buffer;
    uname(&buffer);

    std::string name, id, pretty_name, version;
    std::ifstream file("/etc/os-release");
    while (file)
    {
        std::string temp;
        std::getline(file, temp);
        if (temp.find("NAME=") == 0)
            name = remove_quotes(temp.substr(5));
        if (temp.find("VERSION=") == 0)
            version = remove_quotes(temp.substr(8));
        if (temp.find("ID=") == 0)
            id = remove_quotes(temp.substr(3));
        if (temp.find("PRETTY_NAME=") == 0)
            pretty_name = remove_quotes(temp.substr(12));
    }
    file.close();

    if (pretty_name.empty() || id == "antergos" || id == "linuxmint" || id == "ubuntu")
        pretty_name = ljh::trim_copy(name + " " + version);

    std::string icon = "unknown";
    std::string hostname = buffer.nodename;
    file.open("/etc/machine-info");
    while (file)
    {
        std::string temp;
        std::getline(file, temp);
        if (temp.find("ICON_NAME=") == 0)
            icon = remove_quotes(temp.substr(10));
        if (temp.find("PRETTY_HOSTNAME=") == 0)
            hostname = remove_quotes(temp.substr(16));
    }
    file.close();

    if (icon == "unknown")
    {
        if (std::filesystem::exists("/sys/class/dmi/id/chassis_type"))
            icon = chassis_type_as_system_icon(std::stoi(read_file("/sys/class/dmi/id/chassis_type")));
    }

    struct ifaddrs *base;
    std::string_view adapter_name;
    getifaddrs(&base);

    for (auto address = base; address != nullptr; address = address->ifa_next)
    {
        if (memcmp(address->ifa_name, "lo", 2) == 0)
            continue;

        if (address->ifa_addr->sa_family == AF_INET)
        {
            adapter_name = address->ifa_name;
            inet_ntop(AF_INET, &ljh::pointer_cast<struct sockaddr_in>(address->ifa_addr)->sin_addr, ip_address, sizeof(ip_address));
            goto GOT_ADDRESS;
        }
    }
    for (auto address = base; address != nullptr; address = address->ifa_next)
    {
        if (memcmp(address->ifa_name, "lo", 2) == 0)
            continue;

        if (address->ifa_addr->sa_family == AF_INET6)
        {
            adapter_name = address->ifa_name;
            inet_ntop(AF_INET6, &ljh::pointer_cast<struct sockaddr_in6>(address->ifa_addr)->sin6_addr, ip_address, sizeof(ip_address));
            goto GOT_ADDRESS;
        }
    }
GOT_ADDRESS:
    for (auto address = base; address != nullptr; address = address->ifa_next)
    {
        if (adapter_name == address->ifa_name && address->ifa_addr->sa_family == AF_PACKET)
        {
            struct sockaddr_ll *s = (struct sockaddr_ll *)(address->ifa_addr);
            sprintf(mac_address, "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
                    s->sll_addr[0], s->sll_addr[1], s->sll_addr[2],
                    s->sll_addr[3], s->sll_addr[4], s->sll_addr[5]);
            break;
        }
    }

    freeifaddrs(base);

    system.hostname = (hostname);
    system.operating_system = (pretty_name);
    system.kernel = (buffer.sysname + std::string{" "} + buffer.release);
    system.architecture = (buffer.machine);
    system.icon = (icon);
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    system.mac_address = (mac_address);
    system.ip_address = (ip_address);

    return std::move(system);
}

inline std::string chassis_type_as_system_icon(int a)
{
    switch (a)
    {
    case 0x01: // Other
    case 0x03: // Desktop
    case 0x04: // Low Profile Desktop
    case 0x05: // Pizza Box
    case 0x06: // Mini Tower
    case 0x07: // Tower
    case 0x0D: // All in One
    case 0x10: // Lunch Box
    case 0x22: // Embedded PC
    case 0x23: // Mini PC
    case 0x24: // Stick PC
        return "computer";

    case 0x08: // Portable
    case 0x09: // Laptop
    case 0x0A: // Notebook
    case 0x0C: // Docking Station
    case 0x0E: // Sub Notebook
    case 0x1F: // Convertible
    case 0x20: // Detachable
        return "computer-laptop";

    case 0x11: // Main Server Chassis
    case 0x12: // Expansion Chassis
    case 0x13: // SubChassis
    case 0x14: // Bus Expansion Chassis
    case 0x15: // Peripheral Chassis
    case 0x16: // RAID Chassis
    case 0x17: // Rack Mount Chassis
    case 0x1C: // Blade
    case 0x1D: // Blade Enclosure
        return "network-server";

    case 0x0B: // Hand Held
        return "phone";

    case 0x1E: // Tablet
        return "tablet";

    case 0x02: // Unknown
    case 0x0F: // Space-saving
    case 0x18: // Sealed-case PC
    case 0x19: // Multi-system chassis
    case 0x1A: // Compact PCI
    case 0x1B: // Advanced TCA
    case 0x21: // IoT Gateway
        return "unknown";
    }

    return "";
}

extern std::tuple<int, std::string> exec(const std::string &cmd);

namespace Control
{
    ljh::expected<void, Errors> Shutdown(const Fields &fields)
    {
#if defined(LJH_TARGET_Windows)
        if (InitiateSystemShutdownExA(nullptr, nullptr, 0, false, false, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER) == 0)
            return ljh::unexpected{Errors::Failed};
        return ljh::expected<void, Errors>{};
#elif defined(LJH_TARGET_Linux)
        auto [exit, _] = exec("poweroff now");
        if (exit != 0)
        {
            std::tie(exit, _) = exec("shutdown now");
            if (exit != 0)
            {
                return ljh::unexpected{Errors::Failed};
            }
        }
        return ljh::expected<void, Errors>{};
#else
        return ljh::unexpected{Errors::NotImplemented};
#endif
    }

    ljh::expected<void, Errors> Reboot(const Fields &fields)
    {
#if defined(LJH_TARGET_Windows)
        if (InitiateSystemShutdownExA(nullptr, nullptr, 0, false, true, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER) == 0)
            return ljh::unexpected{Errors::Failed};
        return ljh::expected<void, Errors>{};
#elif defined(LJH_TARGET_Linux)
        auto [exit, _] = exec("reboot now");
        if (exit != 0)
        {
            std::tie(exit, _) = exec("shutdown -r now");
            if (exit != 0)
            {
                return ljh::unexpected{Errors::Failed};
            }
        }
        return ljh::expected<void, Errors>{};
#else
        return ljh::unexpected{Errors::NotImplemented};
#endif
    }
};