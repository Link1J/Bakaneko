// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "server.h"

#include <QProcess>
#include <QSettings>
#include <QtNetwork>
#include <QIcon>

#include <iostream>
#include <string_view>
#include <thread>

#include "managers/servermanager.h"
#include "managers/settings.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

using namespace std::string_view_literals;

QString Server::get_hostname()
{
    std::string pretty{pretty_hostname.toUtf8().data()};
    if (!pretty.empty())
        return pretty_hostname;
    return hostname;
}

QString Server::get_system_icon()
{
    if (system_icon.isEmpty())
        if (QIcon::hasThemeIcon("computer-fail"))
            return "computer-fail";
        else
            return "computer-fail-symbolic";
    return system_icon;
}

Server::State Server::get_state      () { return state           ;   }
QString       Server::get_os         () { return operating_system;   }
QString       Server::get_ip         () { return ip_address      ;   }
QString       Server::get_mac        () { return mac_address     ;   }
QString       Server::get_system_type() { return system_type     ;   }
QString       Server::get_kernel     () { return kernel          ;   }
QString       Server::get_arch       () { return architecture    ;   }
QString       Server::get_vm_platform() { return vm_platform     ;   }

std::string chassis_type_as_string(int a)
{
    switch(a)
    {
    case 0x01: return "Other";
    case 0x02: return "Unknown";
    case 0x03: return "Desktop";
    case 0x04: return "Low Profile Desktop";
    case 0x05: return "Pizza Box";
    case 0x06: return "Mini Tower";
    case 0x07: return "Tower";
    case 0x08: return "Portable";
    case 0x09: return "Laptop";
    case 0x0A: return "Notebook";
    case 0x0B: return "Hand Held";
    case 0x0C: return "Docking Station";
    case 0x0D: return "All in One";
    case 0x0E: return "Sub Notebook";
    case 0x0F: return "Space-saving";
    case 0x10: return "Lunch Box";
    case 0x11: return "Main Server Chassis";
    case 0x12: return "Expansion Chassis";
    case 0x13: return "SubChassis";
    case 0x14: return "Bus Expansion Chassis";
    case 0x15: return "Peripheral Chassis";
    case 0x16: return "RAID Chassis";
    case 0x17: return "Rack Mount Chassis";
    case 0x18: return "Sealed-case PC";
    case 0x19: return "Multi-system chassis";
    case 0x1A: return "Compact PCI";
    case 0x1B: return "Advanced TCA";
    case 0x1C: return "Blade";
    case 0x1D: return "Blade Enclosure";
    case 0x1E: return "Tablet";
    case 0x1F: return "Convertible";
    case 0x20: return "Detachable";
    case 0x21: return "IoT Gateway";
    case 0x22: return "Embedded PC";
    case 0x23: return "Mini PC";
    case 0x24: return "Stick PC";
    }
    return "";
}

std::string chassis_type_as_system_type(int a)
{
    auto data = chassis_type_as_string(a);
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return std::tolower(c); });
    return data;
}

std::string chassis_type_as_system_icon(int a)
{
    switch(a)
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

// Note: pings to Windows computers fail if "File and Printer Sharing" is not enabled
// Fix here, https://superuser.com/a/1137913
void Server::update_info()
{
#if defined(_WIN32)
    static char SendData[32] = "Data Buffer";
    static auto hIcmpFile = IcmpCreateFile();

    IPAddr ipaddr;
    inet_pton(AF_INET, ip_address.toLocal8Bit().data(), &ipaddr);
    
    DWORD ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    auto ReplyBuffer = (void*)malloc(ReplySize);
    auto dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);
    free(ReplyBuffer);

    auto new_state = dwRetVal != 0 ? State::Online : State::Offline;
#else
    std::string command = "ping -c 1 " + std::string{ip_address.toLocal8Bit().data()} + " 1>/dev/null 2>&1";
    State new_state = system(command.c_str()) == 0 ? State::Online : State::Offline;
#endif

    if (new_state != State::Offline)
    {
        //check_for_updates();
    }

    if (new_state == state)
        return;
    state = new_state;
    Q_EMIT changed_state();

    if (new_state == State::Offline)
    {
        Q_EMIT this_offline(this);
        system_icon = "";
        Q_EMIT changed_system_icon();
        return;
    }

    Q_EMIT this_online(this);

    int exit_code;
    std::string std_out;
    std::string std_err;

    if (kernal_type == "Linux")
    {
        std::tie(exit_code, std_out, std_err) = run_command("hostnamectl status");

        if (exit_code != 0)
        {
            system_icon = "";
            Q_EMIT changed_system_icon();
            return;
        }

        auto get_value = [std_out](std::string key) -> std::string {
            auto text_pos = std_out.find(key) + key.length() + 2;
            if (text_pos == std::string::npos + key.length() + 2)
                return "";
            auto newline_pos = std_out.find('\n', text_pos);
            return std_out.substr(text_pos, newline_pos - text_pos);
        };

        operating_system = QString::fromStdString(get_value("Operating System"));
        kernel           = QString::fromStdString(get_value("Kernel"          ));
        system_icon      = QString::fromStdString(get_value("Icon name"       ));
        system_type      = QString::fromStdString(get_value("Chassis"         ));
        architecture     = QString::fromStdString(get_value("Architecture"    ));
        pretty_hostname  = QString::fromStdString(get_value("Pretty hostname" ));
        vm_platform      = QString::fromStdString(get_value("Virtualization"  ));

        Q_EMIT changed_system_icon();
        Q_EMIT changed_system_type();
        Q_EMIT changed_os         ();
        Q_EMIT changed_kernel     ();
        Q_EMIT changed_arch       ();
        Q_EMIT changed_hostname   ();
        Q_EMIT changed_vm_platform();
        ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
    }
    else if (kernal_type == "Windows")
    {
        auto get_info = [this](std::string clazz, std::string key) -> std::string {
            auto [exit_code, std_out, std_err] = run_command("wmic " + clazz + " get " + key);
            if (exit_code != 0)
                return "";
            auto newline_pos1 = std_out.find('\n') + 1;
            auto newline_pos2 = std_out.find_last_not_of(' ', std_out.find('\r', newline_pos1) - 1) + 1;
            return std_out.substr(newline_pos1, newline_pos2 - newline_pos1);
        };

        auto get_env_var = [this](std::string var) -> std::string {
            auto [exit_code, std_out, std_err] = run_command("echo %" + var + "%");
            if (std_out.find("%"+var+"%") != std::string::npos)
                std::tie(exit_code, std_out, std_err) = run_command("echo $env:" + var);
            if (exit_code != 0)
                return "";
            return std_out.substr(0, std_out.find('\n'));
        };

        auto get_reg_value = [this](std::string path, std::string key) -> std::string {
            auto command = "reg query \\\"" + path + "\\\" /v " + key;
            auto [exit_code, std_out, std_err] = run_command(command);
            if (exit_code != 0)
                return "";
            auto start = std_out.find("REG_");
            start = std_out.find(' ', start);
            start = std_out.find_first_not_of(' ', start);
            auto length = std_out.find("\r", start) - start;
            return std_out.substr(start, length);
        };

        auto os_name = get_info("OS", "Caption");
        if (os_name.find("Microsoft ") == 0)
            os_name = os_name.substr(10);

        if (int line = os_name.find("Windows 10"); line != std::string::npos)
        {
            line += 10;
            auto version = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "DisplayVersion");
            if (version == "")
                version = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ReleaseId");
            if (version != "")
                os_name = os_name.substr(0, line + 1) + version + os_name.substr(line);
        }
        else
        {
            line += os_name.find_first_of(' ', line + 11);
            auto version = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CSDVersion");
            if (version != "")
                os_name = os_name.substr(0, line + 1) + version + os_name.substr(line);
        }

        operating_system = QString::fromStdString(os_name);

        auto kernel_version = "NT " + get_info("OS", "Version");
        auto UBR = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "UBR");
        if (UBR != "") kernel_version += "." + std::to_string(std::stoi(UBR, nullptr, 0));

        kernel           = QString::fromStdString(kernel_version);

        architecture     = QString::fromStdString(get_env_var("PROCESSOR_ARCHITECTURE"));

        int chassis_type = std::stoi(get_info("SystemEnclosure", "ChassisTypes").substr(1));
        system_icon      = QString::fromStdString(chassis_type_as_system_icon(chassis_type));
        system_type      = QString::fromStdString(chassis_type_as_system_type(chassis_type));

        Q_EMIT changed_system_icon();
        Q_EMIT changed_system_type();
        Q_EMIT changed_os         ();
        Q_EMIT changed_kernel     ();
        Q_EMIT changed_arch       ();
        ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
    }
}

Server::Server(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QString kernal_type, QObject* parent)
    : QObject(parent)
    , hostname   {hostname   }
    , ip_address {ip_address }
    , mac_address{mac_address}
    , username   {username   }
    , password   {password   }
    , kernal_type{kernal_type}
{
    state = State::Unknown;
}

void Server::wake_up()
{
    std::string mac = mac_address.toUtf8().data();
    std::string ip  = ip_address .toUtf8().data();

    auto char_to_hex = [](char letter) -> char {
        if (isdigit(letter))
            return letter - '0';
        else if (letter >= 'a' && letter <= 'f') 
            return letter - 'a' + 10;
        else if (letter >= 'A' && letter <= 'F') 
            return letter - 'A' + 10;
        return '\xFF';
    };

    std::string dest(6, (char)0);

    sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dest[0], &dest[1], &dest[2], &dest[3], &dest[4], &dest[5]);

    std::string magic_packet(6, '\xFF');
    for (int a = 0; a < 16; a++)
    {
        magic_packet += dest;
    }

    auto packet = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

#if defined(_WIN32)
    ULONG ip_address_size = 0;
    GetAdaptersAddresses(AF_UNSPEC, 0, NULL, nullptr, &ip_address_size);
    IP_ADAPTER_ADDRESSES* ip_address = (IP_ADAPTER_ADDRESSES*)malloc(ip_address_size);
    GetAdaptersAddresses(AF_UNSPEC, 0, NULL, ip_address, &ip_address_size);

    sockaddr name = {0};
    int name_length = 0;
    for (auto adapter = ip_address; adapter != nullptr; adapter = adapter->Next)
    {
        for (auto address = adapter->FirstUnicastAddress; address != nullptr; address = address->Next)
        {
            if (address->Address.lpSockaddr->sa_family == AF_INET)
            {
                name = *address->Address.lpSockaddr;
                name_length = address->Address.iSockaddrLength;
                break;
            }
        }
        if (name.sa_family != 0)
            break;
    }

    bind(packet, &name, name_length);
#endif

    const int optval{1};
    if (setsockopt(packet, SOL_SOCKET, SO_BROADCAST, (const char*)&optval, sizeof(optval)) < 0) {
        throw std::runtime_error("Failed to set socket options");
    }

    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = 0xFFFFFFFF;
    addr.sin_port = htons(Settings::Instance().get_wol_port());

    // Send the packet out. 
    if (sendto(packet, magic_packet.c_str(), (int)magic_packet.length(), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        throw std::runtime_error("Failed to send packet");
    }

#if defined(_WIN32)
    closesocket(packet);
#else
    close(packet);
#endif
}

/*
    For shutdown to work, sudo must be installed and the line below added to /etc/sudoers
    user hostname =NOPASSWD: /usr/bin/systemctl poweroff,/usr/bin/systemctl reboot
*/
void Server::shutdown()
{
    if (kernal_type == "Linux")
    {
        run_command("sudo systemctl poweroff");
    }
    else if (kernal_type == "Windows")
    {
        run_command("shutdown /s /t 0");
    }
}

std::tuple<int, std::string, std::string> Server::run_command(std::string command)
{
    auto connection = get_ssh_connection();

    std::string std_out;
    std::string std_err;

    char buffer[256];
    int nbytes;
    int volatile code;

    while ((code = ssh_channel_request_exec(connection, command.c_str())) == SSH_AGAIN);
    if (code == SSH_OK)
    {
        memset(buffer, 0, sizeof(buffer));
        while ((nbytes = ssh_channel_read(connection, buffer, sizeof(buffer), false)) > 0)
            std_out += std::string(buffer, nbytes);

        memset(buffer, 0, sizeof(buffer));
        while ((nbytes = ssh_channel_read(connection, buffer, sizeof(buffer), true)) > 0)
            std_err += std::string(buffer, nbytes);

        code = ssh_channel_get_exit_status(connection);
    }
    else
    {
        std::cerr << ssh_get_error((ssh_session)connection) << "\n";
    }

    return {code, std_out, std_err};
}

/*
    For shutdown to work, sudo must be installed and the line below added to /etc/sudoers
    user hostname =NOPASSWD: /usr/bin/systemctl poweroff,/usr/bin/systemctl reboot
*/
void Server::reboot()
{
    if (kernal_type == "Linux")
    {
        run_command("sudo systemctl reboot");
    }
    else if (kernal_type == "Windows")
    {
        run_command("shutdown /r /t 0");
    }
}

// pacman -Qu


ssh_connection Server::get_ssh_connection()
{
    auto session = ssh_new();

    ssh_options_set(session, SSH_OPTIONS_HOST, ip_address.toUtf8().data());
    ssh_options_set(session, SSH_OPTIONS_USER, username.toUtf8().data());

    ssh_connect(session);

    ssh_userauth_password(session, NULL, password.toUtf8().data());
    
    auto channel = ssh_channel_new(session);
    ssh_channel_open_session(channel);

    return {session, channel};
}

Server::~Server() = default;