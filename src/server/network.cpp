// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"

#include <filesystem>
#include <chrono>

#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>

#include <spdlog/fmt/fmt.h>

#if defined(LJH_TARGET_Windows)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include "windows.hpp"
#elif defined(LJH_TARGET_Linux)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#endif

extern std::string read_file(std::filesystem::path file_path);

ljh::expected<Bakaneko::Adapters, Errors> Info::Adapters(const Fields& fields)
{
    Bakaneko::Adapters adapters;

#if defined(LJH_TARGET_Windows)
    auto info_adapters = Win32Run(GetAdaptersInfo);

    for (auto info_adapter = info_adapters.get(); info_adapter != nullptr; info_adapter = info_adapter->Next)
    {
        using namespace std::literals;

        MIB_IF_ROW2 item = {0};
        item.InterfaceIndex = info_adapter->Index;
        GetIfEntry2(&item);
        auto time = std::chrono::steady_clock::now();
        if (item.PhysicalMediumType != NdisPhysicalMediumNative802_11 && item.PhysicalMediumType != NdisPhysicalMediumWirelessLan && item.PhysicalMediumType != NdisPhysicalMedium802_3 && item.PhysicalMediumType != NdisPhysicalMediumUnspecified)
            continue;

        auto adapter = adapters.add_adapter();

        adapter->set_name(ljh::convert_string(item.Alias));

        adapter->set_mac_address(fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
            info_adapter->Address[0], info_adapter->Address[1], info_adapter->Address[2], info_adapter->Address[3],
            info_adapter->Address[4], info_adapter->Address[5], info_adapter->Address[6], info_adapter->Address[7]
        ));

        if (item.OperStatus == IfOperStatusUp)
        {
            adapter->set_state(Bakaneko::Adapter_State_Up);
            adapter->set_ip_address(info_adapter->IpAddressList.IpAddress.String);
        }

        adapter->set_time(time.time_since_epoch().count());
        adapter->set_bytes_rx(item.InOctets);
        adapter->set_bytes_tx(item.OutOctets);
        adapter->set_mtu(item.Mtu);
        adapter->set_link_speed(item.TransmitLinkSpeed);
    }
#elif defined(LJH_TARGET_Linux)
    for (auto& adapter_file : std::filesystem::directory_iterator{"/sys/class/net"})
    {
        auto adapter_path = adapter_file.path();
        if (!std::filesystem::exists(adapter_path/"type"))
            continue;
        if (read_file(adapter_path/"type") == "772")
            continue;
        auto adapter = adapters.add_adapter();

        adapter->set_name(adapter_path.filename().string());

        adapter->set_mac_address(read_file(adapter_path/"address"));

        auto state = read_file(adapter_path/"operstate");
        if (state == "up")
        {
            adapter->set_state(Bakaneko::Adapter_State_Up);

            auto fd = socket(AF_INET, SOCK_DGRAM, 0);
            struct ifreq ifr;
            ifr.ifr_addr.sa_family = AF_INET;
            strncpy(ifr.ifr_name, adapter_path.filename().c_str(), IFNAMSIZ);
            ioctl(fd, SIOCGIFADDR, &ifr);
            close(fd);

            adapter->set_ip_address(inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
        }

        auto rx = read_file(adapter_path/"statistics"/"rx_bytes");
        auto tx = read_file(adapter_path/"statistics"/"tx_bytes");
        auto time = std::chrono::steady_clock::now();

        adapter->set_time(time.time_since_epoch().count());
        adapter->set_bytes_rx(std::stoull(rx));
        adapter->set_bytes_tx(std::stoull(tx));
        
        adapter->set_mtu(std::stoull(read_file(adapter_path/"mtu")));

        if (auto speed = read_file(adapter_path/"speed"); !speed.empty() && speed != "-1")
            adapter->set_link_speed(std::stoull(speed) * 1000000);
    }
#endif
    return adapters;
}