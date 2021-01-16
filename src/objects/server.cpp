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

#include "servers/windows.h"
#include "servers/linux.h"

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

Server::State Server::get_state      () { return state           ; }
QString       Server::get_os         () { return operating_system; }
QString       Server::get_ip         () { return ip_address      ; }
QString       Server::get_mac        () { return mac_address     ; }
QString       Server::get_system_type() { return system_type     ; }
QString       Server::get_kernel     () { return kernel          ; }
QString       Server::get_arch       () { return architecture    ; }
QString       Server::get_vm_platform() { return vm_platform     ; }
UpdateList    Server::get_updates    () { return updates         ; }

void Server::update_info()
{
    std::lock_guard __lock_guard{update_lock};
    
    auto new_state = ping_computer();

    if (new_state == state)
    {
        if (state == State::Online)
        {
            check_for_updates();
        }
        return;
    }
    else if (state != State::Unknown)
    {
        if (new_state == State::Offline)
        {
            Q_EMIT this_offline(this);
        }
        else
        {
            Q_EMIT this_online(this);
        }
    }

    state = new_state;
    Q_EMIT changed_state();

    if (new_state == State::Offline)
    {
        system_icon = "";
        Q_EMIT changed_system_icon();
        return;
    }

    collect_info();
}

// Note: pings to Windows computers fail if "File and Printer Sharing" is not enabled
// Fix here, https://superuser.com/a/1137913
Server::State Server::ping_computer()
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

    return dwRetVal != 0 ? State::Online : State::Offline;
#else
    std::string command = "ping -c 1 " + std::string{ip_address.toLocal8Bit().data()} + " 1>/dev/null 2>&1";
    return system(command.c_str()) == 0 ? State::Online : State::Offline;
#endif
}

Server::Server(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QObject* parent)
    : QObject(parent)
    , hostname   {hostname   }
    , ip_address {ip_address }
    , mac_address{mac_address}
    , username   {username   }
    , password   {password   }
{
    state = State::Unknown;
}

void Server::wake_up()
{
    std::string mac = mac_address.toUtf8().data();
    std::string ip  = ip_address .toUtf8().data();
    std::string dest(6, (char)0);

    sscanf(mac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dest[0], &dest[1], &dest[2], &dest[3], &dest[4], &dest[5]);

    std::string magic_packet(6, '\xFF');
    for (int a = 0; a < 16; a++)
        magic_packet += dest;

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

static std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

class Temp : public Server
{
public:
    explicit Temp(QString ip_address, QString username, QString password)
        : Server("", "", ip_address, username, password, nullptr)
    {}

    QString get_kernal_type() override { return ""; }
    void check_for_updates () override {            }
    void collect_info      () override {            }
    void shutdown          () override {            }
    void reboot            () override {            }
};

Server* Server::create(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QString kernal_type)
{
    if (kernal_type.isEmpty())
    {
        auto temp = new Temp(ip_address, username, password);
        auto [exit_code, std_out, std_err] = temp->run_command("uname");
        delete temp;

        if (exit_code != 0)
        {
            kernal_type = "Windows";
        }
        else
        {
            kernal_type = QString::fromStdString(std_out.substr(0, std_out.size() - 1));
        }
    }

    if (kernal_type == "Windows")
    {
        return new WindowsComputer(hostname, mac_address, ip_address, username, password);
    }
    if (kernal_type == "Linux")
    {
        return new LinuxComputer(hostname, mac_address, ip_address, username, password);
    }
    return nullptr;
}