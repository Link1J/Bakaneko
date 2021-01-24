// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "server.h"

#include <QProcess>
#include <QSettings>
#include <QtNetwork>
#include <QIcon>
#include <QtConcurrent>

#include <iostream>
#include <string_view>
#include <thread>

#include "managers/servermanager.h"
#include "managers/settings.h"

#include <ljh/function_traits.hpp>

#include "windows.hpp"

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

asio::io_context ioctx;

QString Server::get_icon()
{
    if (system_info.icon().empty())
    {
        if (QIcon::hasThemeIcon("computer-fail"))
            return "computer-fail";
        else
            return "computer-fail-symbolic";
    }
    return QString::fromStdString(system_info.icon());
}

Server::State Server::get_state() { return state; }

QString Server::get_hostname() { return QString::fromStdString(hostname   ); }
QString Server::get_ip      () { return QString::fromStdString(ip_address ); }
QString Server::get_mac     () { return QString::fromStdString(mac_address); }

QString Server::get_os    () { return QString::fromStdString(system_info.operating_system()); }
QString Server::get_arch  () { return QString::fromStdString(system_info.architecture    ()); }
QString Server::get_kernel() { return QString::fromStdString(system_info.kernel          ()); }

UpdateModel* Server::get_updates() { return &updates; }
DrivesModel* Server::get_drives () { return &drives ; }

void Server::update_info()
{
    std::lock_guard __lock_guard{update_lock};
    if (steps_done != max_steps) return;
    steps_done = 0;
    
    auto new_state = ping_computer();

    if (new_state == state)
    {
        if (state == State::Online)
        {
            steps_done++;
            network_get("/drives" , &Server::got_drives );
            network_get("/updates", &Server::got_updates);
        }
        else
        {
            steps_done = max_steps;
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
        steps_done = max_steps;
        return;
    }

    network_get("/system" , &Server::got_info   );
    network_get("/drives" , &Server::got_drives );
    network_get("/updates", &Server::got_updates);
}

asio::ip::tcp::socket& Server::connection()
{
    if (!socket.is_open())
    {
        asio::ip::tcp::resolver resolver(ioctx);
        auto const results = resolver.resolve(ip_address, "8080");
        asio::connect(socket, results.begin(), results.end());
    }
    return socket;
}

Server::State Server::ping_computer()
{
    try
    {
        std::lock_guard _{socket_lock};
        
        beast::http::request<beast::http::string_body> req{beast::http::verb::head, "/", 11};

        req.set(beast::http::field::host, ip_address);
        req.version(11);

        beast::http::write(connection(), req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::empty_body> res;

        beast::http::read(connection(), buffer, res);

        if (res.result_int() == 302)
            return Server::State::Online;
    }
    catch(const boost::wrapexcept<boost::system::system_error>& e)
    {
        connection().close();
    }
    return Server::State::Offline;
}

Server::Server(std::string hostname, std::string mac_address, std::string ip_address, QObject* parent)
    : QObject(parent)
    , hostname   {hostname   }
    , ip_address {ip_address }
    , mac_address{mac_address}
    , socket     {ioctx      }
    , updates    {this       }
{
    state = State::Unknown;
    connect(this, &Server::got_info   , this, &Server::handle_info   );
    connect(this, &Server::got_drives , this, &Server::handle_drives );
    connect(this, &Server::got_updates, this, &Server::handle_updates);
}

void Server::wake_up()
{
    std::string dest(6, (char)0);

    sscanf(mac_address.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dest[0], &dest[1], &dest[2], &dest[3], &dest[4], &dest[5]);

    std::string magic_packet(6, '\xFF');
    for (int a = 0; a < 16; a++)
        magic_packet += dest;

    auto packet = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

#if defined(_WIN32)

    auto ip_address = Win32Run(GetAdaptersAddresses, AF_UNSPEC, 0, nullptr);
    sockaddr name = {0};
    int name_length = 0;
    for (auto adapter = &ip_address[0]; adapter != nullptr; adapter = adapter->Next)
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

    // ssh_options_set(session, SSH_OPTIONS_HOST, ip_address.toUtf8().data());
    // ssh_options_set(session, SSH_OPTIONS_USER, username.toUtf8().data());

    ssh_connect(session);

    // ssh_userauth_password(session, NULL, password.toUtf8().data());
    
    auto channel = ssh_channel_new(session);
    ssh_channel_open_session(channel);

    return {session, channel};
}

template<typename T>
void Server::network_get(std::string path, void(Server::*signal)(T))
{
    QtConcurrent::run([this, path, signal]{
        try
        {
            std::lock_guard _{socket_lock};

            beast::http::request<beast::http::string_body> req{beast::http::verb::get, path, 11};

            req.set(beast::http::field::host, ip_address);
            req.set(beast::http::field::content_type, "application/x-protobuf");
            req.version(11);

            beast::http::write(connection(), req);

            beast::flat_buffer buffer;
            beast::http::response<beast::http::string_body> res;

            beast::http::read(connection(), buffer, res);
            
            T info;

            if (res.result_int() != 200) throw 1;
            if (info.GetTypeName() != res["Protobuf-Type"]) throw 1;

            info.ParseFromString(res.body());
            Q_EMIT (*this.*signal)(info);
        }
        catch (...)
        {
            steps_done++;
        }
    });
}

void Server::handle_info(Bakaneko::System info)
{
    system_info = info;
    Q_EMIT changed_os();
    Q_EMIT changed_icon();
    Q_EMIT changed_kernel();
    Q_EMIT changed_arch();
    steps_done++;
}

void Server::handle_updates(Bakaneko::Updates updates_info)
{
    if (updates.rowCount() < updates_info.update_size())
        updates.insertRows(updates.rowCount(), updates_info.update_size() - updates.rowCount());
    if (updates.rowCount() > updates_info.update_size())
        updates.removeRows(updates_info.update_size(), updates.rowCount() - updates_info.update_size());

    for (int a = 0; a < updates_info.update_size(); a++)
    {
        if (updates.data(a, UpdateModel::NameRole) != updates_info.update()[a].name())
        {
            updates.setData(a, updates_info.update()[a].name       (), UpdateModel::NameRole      );
            updates.setData(a, updates_info.update()[a].old_version(), UpdateModel::OldVersionRole);
            updates.setData(a, updates_info.update()[a].new_version(), UpdateModel::NewVersionRole);
        }
        else if (updates.data(a, UpdateModel::NewVersionRole) != updates_info.update()[a].new_version())
        {
            updates.setData(a, updates_info.update()[a].new_version(), UpdateModel::NewVersionRole);
        }
    }

    steps_done++;
}

void Server::handle_drives(Bakaneko::Drives info)
{
    std::vector vinfo(info.drive().begin(), info.drive().end());

    for (int a = 0; a < drives.rowCount(); a++)
    {
        bool found = false;
        for (auto& dinfo : info.drive())
            if (drives.data(a).dev_node() == dinfo.dev_node())
                found = true;
        if (!found)
        {
            drives.removeRow(a);
            a--;
        }
    }

    auto update_partitions = [this](int idnex, const Bakaneko::Drive& dinfo) {
        auto& partitions = drives.partition(idnex);
        auto& ipartition = dinfo .partition(     );

        if (partitions.rowCount() < dinfo.partition_size())
            partitions.insertRows(partitions.rowCount(), dinfo.partition_size() - partitions.rowCount());
        if (partitions.rowCount() > dinfo.partition_size())
            partitions.removeRows(dinfo.partition_size(), partitions.rowCount() - dinfo.partition_size());

        for (int a = 0; a < dinfo.partition_size(); a++)
        {
            auto& pdata = partitions.data(a);
            auto& pinfo = ipartition     [a];

            if (pdata.dev_node() != pinfo.dev_node())
            {
                pdata = pinfo;
                partitions.flag(a, {
                    PartitionModel::ROLE_dev_node,
                    PartitionModel::ROLE_size,
                    PartitionModel::ROLE_used,
                    PartitionModel::ROLE_mountpoint,
                    PartitionModel::ROLE_filesystem,
                });
            }
            else
            {
                if (pdata.size() != pinfo.size())
                {
                    pdata.set_size(pinfo.size());
                    partitions.flag(a, { PartitionModel::ROLE_size });
                }
                if (pdata.used() != pinfo.used())
                {
                    pdata.set_used(pinfo.used());
                    partitions.flag(a, { PartitionModel::ROLE_used });
                }
                if (pdata.mountpoint() != pinfo.mountpoint())
                {
                    pdata.set_mountpoint(pinfo.mountpoint());
                    partitions.flag(a, { PartitionModel::ROLE_mountpoint });
                }
                if (pdata.filesystem() != pinfo.filesystem())
                {
                    pdata.set_filesystem(pinfo.filesystem());
                    partitions.flag(a, { PartitionModel::ROLE_filesystem });
                }
            }
        }
    };

    for (auto& dinfo : info.drive())
    {
        bool done = false;
        for (int a = 0; a < drives.rowCount(); a++)
        {
            auto& drive = drives.data(a);
            if (drive.dev_node() == dinfo.dev_node())
            {
                update_partitions(a, dinfo);
                done = true;
            }
        }
        if (!done)
        {
            drives.insertRow(drives.rowCount());
            auto& drive = drives.data(drives.rowCount() - 1);
            drive.set_dev_node    (dinfo.dev_node    ());
            drive.set_size        (dinfo.size        ());
            drive.set_model       (dinfo.model       ());
            drive.set_manufacturer(dinfo.manufacturer());
            drive.set_interface   (dinfo.interface   ());
            drives.flag(drives.rowCount() - 1, {
                DrivesModel::ROLE_dev_node,
                DrivesModel::ROLE_size,
                DrivesModel::ROLE_model,
                DrivesModel::ROLE_manufacturer,
                DrivesModel::ROLE_interface,
            });
            update_partitions(drives.rowCount() - 1, dinfo);
        }
    }

    steps_done++;
}

void Server::shutdown() {}
void Server::reboot() {}

Server::~Server()
{
    boost::system::error_code ec;
    socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
}
