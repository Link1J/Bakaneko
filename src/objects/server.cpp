// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#include "server.h"

#include <QProcess>
#include <QSettings>
#include <QtNetwork>

#include <iostream>
#include <string_view>

#include <libssh/libssh.h>

#include "managers/servermanager.h"

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

Server::State Server::get_state      () { return state           ;   }
QString       Server::get_os         () { return operating_system;   }
QString       Server::get_ip         () { return ip_address      ;   }
QString       Server::get_mac        () { return mac_address     ;   }
QString       Server::get_system_icon() { return system_icon     ;   }
QString       Server::get_system_type() { return system_type     ;   }
QString       Server::get_kernel     () { return kernel          ;   }
QString       Server::get_arch       () { return architecture    ;   }
QString       Server::get_vm_platform() { return vm_platform     ;   }

void Server::update_info()
{
#if defined(_WIN32)
    char SendData[32] = "Data Buffer";

    IPAddr ipaddr;
    inet_pton(AF_INET, ip_address.toLocal8Bit().data(), &ipaddr);
    auto hIcmpFile = IcmpCreateFile();

    DWORD ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    auto ReplyBuffer = (void*)malloc(ReplySize);

    auto dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, 50);

    auto new_state = dwRetVal != 0 ? State::Online : State::Offline;
    
    IcmpCloseHandle(hIcmpFile);
#else
    std::string command = "ping -c 1 " + std::string{ip_address.toLocal8Bit().data()} + " 1>/dev/null 2>&1";
    State new_state = system(command.c_str()) == 0 ? State::Online : State::Offline;
#endif

    if (new_state == state)
        return;
    state = new_state;
    Q_EMIT changed_state();

    if (new_state == State::Offline)
    {
        operating_system = kernel = system_icon = system_type = architecture = vm_platform = pretty_hostname = "";
        Q_EMIT changed_system_icon();
        Q_EMIT changed_system_type();
        Q_EMIT changed_os         ();
        Q_EMIT changed_kernel     ();
        Q_EMIT changed_arch       ();
        Q_EMIT changed_vm_platform();
        Q_EMIT changed_hostname   ();
        ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
        return;
    }

    int exit_code;
    std::string std_out;
    std::string std_err;
    std::tie(exit_code, std_out, std_err) = run_command("hostnamectl status");

    if (exit_code != 0)
    {
        operating_system = kernel = system_icon = system_type = architecture = vm_platform = pretty_hostname = "";
        Q_EMIT changed_system_icon();
        Q_EMIT changed_system_type();
        Q_EMIT changed_os         ();
        Q_EMIT changed_kernel     ();
        Q_EMIT changed_arch       ();
        Q_EMIT changed_vm_platform();
        Q_EMIT changed_hostname   ();
        ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
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
    vm_platform      = QString::fromStdString(get_value("Virtualization" ));

    Q_EMIT changed_system_icon();
    Q_EMIT changed_system_type();
    Q_EMIT changed_os         ();
    Q_EMIT changed_kernel     ();
    Q_EMIT changed_arch       ();
    Q_EMIT changed_hostname   ();
    Q_EMIT changed_vm_platform();
    ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
}

Server::Server(QObject* parent)
    : QObject(parent)
{
    timer = std::make_shared<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, &Server::update_info);
    timer->start(5000);
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
    addr.sin_port = htons(9);

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
    run_command("sudo systemctl poweroff");
}

std::tuple<int, std::string, std::string> Server::run_command(std::string command)
{
    auto session = ssh_new();

    ssh_options_set(session, SSH_OPTIONS_HOST, ip_address.toUtf8().data());
    ssh_options_set(session, SSH_OPTIONS_USER, username.toUtf8().data());

    ssh_connect(session);
    ssh_userauth_password(session, NULL, password.toUtf8().data());

    auto channel = ssh_channel_new(session);
    ssh_channel_open_session(channel);

    std::string std_out;
    std::string std_err;

    char buffer[256];
    int nbytes;
    int volatile code;

    while ((code = ssh_channel_request_exec(channel, command.c_str())) == SSH_AGAIN);
    if (code == SSH_OK)
    {
        memset(buffer, 0, sizeof(buffer));
        while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), false)) > 0)
            std_out += std::string(buffer, nbytes);

        memset(buffer, 0, sizeof(buffer));
        while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), true)) > 0)
            std_err += std::string(buffer, nbytes);

        code = ssh_channel_get_exit_status(channel);
    }
    else
    {
        std::cerr << ssh_get_error(session) << "\n";
    }

    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);

    return {code, std_out, std_err};
}

// pacman -Qu
