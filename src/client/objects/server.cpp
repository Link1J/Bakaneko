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

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>


namespace asio = boost::asio;
namespace beast = boost::beast;

using namespace std::string_view_literals;

static asio::io_context io_context;
// static asio::io_service io_service;

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
    {
        if (QIcon::hasThemeIcon("computer-fail"))
            return "computer-fail";
        else
            return "computer-fail-symbolic";
    }
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
QList<Drive*> Server::get_drives     () { return drives          ; }

void Server::update_info()
{
    std::lock_guard __lock_guard{update_lock};
    
    auto new_state = ping_computer();

    // if (new_state == state)
    // {
    //     if (state == State::Online)
    //     {
    //         check_for_updates();
    //     }
    //     return;
    // }
    // else if (state != State::Unknown)
    // {
    //     if (new_state == State::Offline)
    //     {
    //         Q_EMIT this_offline(this);
    //     }
    //     else
    //     {
    //         Q_EMIT this_online(this);
    //     }
    // }

    state = new_state;
    Q_EMIT changed_state();

    // if (new_state == State::Offline)
    // {
    //     system_icon = "";
    //     Q_EMIT changed_system_icon();
    //     return;
    // }
    // 
    // collect_info();
    // collect_drives();
    // check_for_updates();
}

Server::State Server::ping_computer()
{
    try
    {
        beast::http::request<beast::http::string_body> req{ beast::http::verb::post, "/", 11 };
        std::string ip = ip_address.toUtf8().data();

        req.set(beast::http::field::host, ip);
        req.version(11);
        req.set(beast::http::field::connection, "close");

        asio::ip::tcp::resolver resolver(io_context);
        // beast::tcp_stream stream(io_context);
        // stream.connect(resolver.resolve(ip, "8080"));

        asio::ip::tcp::socket socket{io_context};
        auto const results = resolver.resolve(ip, "8080");
        asio::connect(socket, results.begin(), results.end());

        beast::http::write(socket, req);
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(socket, buffer, res);

        boost::system::error_code ec;
        socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

        if (res.result_int() == 302)
            return Server::State::Online;
    }
    catch(const boost::wrapexcept<boost::system::system_error>& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return Server::State::Offline;
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

class Temp : public Server
{
public:
    explicit Temp(QString ip_address, QString username, QString password)
        : Server("", "", ip_address, username, password, nullptr)
    {}

    QString get_kernal_type() override { return ""; }
    void check_for_updates () override {            }
    void collect_info      () override {            }
    void collect_drives    () override {            }
    void shutdown          () override {            }
    void reboot            () override {            }
};

Server* Server::create(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QString kernal_type)
{
    return new WindowsComputer(hostname, mac_address, ip_address, username, password);
    
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
