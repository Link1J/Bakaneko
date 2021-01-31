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

#include "term/term.h"
#include "term/pty.h"

#include <ljh/function_traits.hpp>

#include "windows.hpp"
#include "bakaneko-version.h"

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

UpdateModel * Server::get_updates () { return &updates ; }
DrivesModel * Server::get_drives  () { return &drives  ; }
AdapterModel* Server::get_adapters() { return &adapters; }

void Server::update_info()
{
    if (!steps_done.try_wait())
        return;
    steps_done.reset(max_steps);
    
    auto new_state = ping_computer();

    if (new_state == state)
    {
        if (state == State::Online)
        {
            steps_done.count_down();
            network_get("/drives"          , &Server::got_drives  );
            network_get("/updates"         , &Server::got_updates );
            network_get("/network/adapters", &Server::got_adapters);
        }
        else
        {
            for (int a = 0; a < max_steps; a++)
                steps_done.count_down();
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
        if (hostname != defualt_hostname)
        {
            hostname = defualt_hostname;
            Q_EMIT changed_hostname();
        }
        for (int a = 0; a < max_steps; a++)
            steps_done.count_down();
        return;
    }

    network_get("/system"          , &Server::got_info    );
    network_get("/drives"          , &Server::got_drives  );
    network_get("/updates"         , &Server::got_updates );
    network_get("/network/adapters", &Server::got_adapters);
}

asio::ip::tcp::socket Server::connection()
{
    // if (!socket.is_open())
    // {
        asio::ip::tcp::socket socket(ioctx);
        asio::ip::tcp::resolver resolver(ioctx);
        auto const results = resolver.resolve(ip_address, "29921");
        asio::connect(socket, results.begin(), results.end());
        return socket;
    // }
    // return socket;
}

Server::State Server::ping_computer()
{
    try
    {
        // std::lock_guard _{socket_lock};
        
        beast::http::request<beast::http::string_body> req{beast::http::verb::head, "/", 11};

        req.set(beast::http::field::host, ip_address);
        req.set(beast::http::field::user_agent, "Bakaneko/" BAKANEKO_VERSION_STRING);
        req.version(11);
        req.keep_alive(true);

        auto channel = connection();

        beast::http::write(channel, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::empty_body> res;

        beast::http::read(channel, buffer, res);

        if (res.result_int() == 302)
            return Server::State::Online;
    }
    catch(const boost::wrapexcept<boost::system::system_error>& e)
    {
    }
    return Server::State::Offline;
}

Server::Server(std::string hostname, std::string mac_address, std::string ip_address, QObject* parent)
    : QObject(parent)
    , hostname        {hostname   }
    , defualt_hostname{hostname   }
    , ip_address      {ip_address }
    , mac_address     {mac_address}
    , socket          {ioctx      }
    , updates         {this       }
    , steps_done      {          0}
{
    state = State::Unknown;
    connect(this, &Server::got_info    , this, &Server::handle_info    );
    connect(this, &Server::got_drives  , this, &Server::handle_drives  );
    connect(this, &Server::got_updates , this, &Server::handle_updates );
    connect(this, &Server::got_adapters, this, &Server::handle_adapters);
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
    for (auto adapter = ip_address.get(); adapter != nullptr; adapter = adapter->Next)
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

template<typename T>
void Server::network_get(std::string path, void(Server::*signal)(T))
{
    QtConcurrent::run([this, path, signal]{
        try
        {
            // std::lock_guard _{socket_lock};

            beast::http::request<beast::http::string_body> req{beast::http::verb::get, path, 11};

            req.set(beast::http::field::host, ip_address);
            req.set(beast::http::field::user_agent, "Bakaneko/" BAKANEKO_VERSION_STRING);
            req.set(beast::http::field::content_type, "application/x-protobuf");
            req.version(11);
            req.keep_alive(true);

            auto channel = connection();

            beast::http::write(channel, req);

            beast::flat_buffer buffer;
            beast::http::response<beast::http::string_body> res;

            beast::http::read(channel, buffer, res);
            
            T info;

            if (res.result_int() != 200) throw 1;
            if (info.GetTypeName() != res["Protobuf-Type"]) throw 1;

            info.ParseFromString(res.body());
            Q_EMIT (*this.*signal)(info);
        }
        catch (...)
        {
            steps_done.count_down();
        }
    });
}

void Server::network_post(std::string path)
{
    QtConcurrent::run([this, path]{
        try
        {
            // std::lock_guard _{socket_lock};

            beast::http::request<beast::http::string_body> req{beast::http::verb::post, path, 11};

            req.set(beast::http::field::host, ip_address);
            req.set(beast::http::field::user_agent, "Bakaneko/" BAKANEKO_VERSION_STRING);
            req.version(11);
            req.keep_alive(true);

            auto channel = connection();

            beast::http::write(channel, req);
        }
        catch (...)
        {
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

    if (hostname != info.hostname())
    {
        hostname = info.hostname();
        Q_EMIT changed_hostname();
    }

    steps_done.count_down();
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

    steps_done.count_down();
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

    steps_done.count_down();
}

void Server::handle_adapters(Bakaneko::Adapters info)
{
    for (int a = 0; a < adapters.rowCount(); a++)
    {
        bool found = false;
        for (auto& dinfo : info.adapter())
            if (adapters.data(a).name() == dinfo.name())
                found = true;
        if (!found)
        {
            adapters.removeRow(a);
            a--;
        }
    }

    for (auto& din : info.adapter())
    {
        bool done = false;
        for (int a = 0; a < adapters.rowCount(); a++)
        {
            auto& cur = adapters.data(a);
            if (cur.name() == din.name())
            {
                auto& pre = adapters.prev(a);
                pre.set_bytes_rx  (cur.bytes_rx  ());
                pre.set_bytes_tx  (cur.bytes_tx  ());
                pre.set_time      (cur.time      ());
                cur.set_bytes_rx  (din.bytes_rx  ());
                cur.set_bytes_tx  (din.bytes_tx  ());
                cur.set_time      (din.time      ());
                cur.set_state     (din.state     ());
                cur.set_link_speed(din.link_speed());
                adapters.flag(a,{
                    AdapterModel::ROLE_link_speed,
                    AdapterModel::ROLE_state,
                    AdapterModel::ROLE_rx_rate,
                    AdapterModel::ROLE_tx_rate,
                });
                done = true;
            }
        }
        if (!done)
        {
            adapters.insertRow(adapters.rowCount());
            adapters.data(adapters.rowCount() - 1) = din;
            auto& pre = adapters.prev(adapters.rowCount() - 1);
            pre.set_bytes_rx(din.bytes_rx());
            pre.set_bytes_tx(din.bytes_tx());
            pre.set_time    (din.time    ());
            drives.flag(drives.rowCount() - 1, {
                AdapterModel::ROLE_name,
                AdapterModel::ROLE_link_speed,
                AdapterModel::ROLE_state,
                AdapterModel::ROLE_mtu,
                AdapterModel::ROLE_mac_address,
                AdapterModel::ROLE_ip_address,
                AdapterModel::ROLE_rx_rate,
                AdapterModel::ROLE_tx_rate,
            });
        }
    }

    steps_done.count_down();
}

void Server::shutdown()
{
    network_post("/power/shutdown");
}

void Server::reboot()
{
    network_post("/power/reboot");
}

Server::~Server()
{
    boost::system::error_code ec;
    socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
}

void Server::open_term(QObject* page, QObject* login, QString username, QString password)
{
    connect(this, SIGNAL(open_term      (QVariant)), page , SLOT(open_term      (QVariant)));
    connect(this, SIGNAL(connecting_fail(QVariant)), login, SLOT(connecting_fail(QVariant)));
    connect(this, SIGNAL(open_term      (QVariant)), login, SLOT(done           (        )));
    
    QtConcurrent::run([this, username, password]{
        auto session = ssh_new();
        if (session == nullptr)
        {
            Q_EMIT connecting_fail(QString::fromUtf8("Failed to create session"));
            return;
        }

        ssh_options_set(session, SSH_OPTIONS_HOST, ip_address.c_str());
        ssh_options_set(session, SSH_OPTIONS_USER, username.toUtf8().data());

        if (ssh_connect(session) != SSH_OK)
        {
            auto message = ssh_get_error(session);
            Q_EMIT connecting_fail(QString::fromUtf8("Could not connect to server"));
            ssh_free(session);
            return;
        }

        if (ssh_userauth_password(session, NULL, password.toUtf8().data()) != SSH_OK)
        {
            auto message = ssh_get_error(session);
            Q_EMIT connecting_fail(QString::fromUtf8(message));
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        auto channel = ssh_channel_new(session);
        if (channel == nullptr)
        {
            auto message = ssh_get_error(session);
            Q_EMIT connecting_fail(QString::fromUtf8(message));
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        if (ssh_channel_open_session(channel) != SSH_OK)
        {
            auto message = ssh_get_error(session);
            Q_EMIT connecting_fail(QString::fromUtf8(message));
            ssh_channel_free(channel);
            ssh_disconnect(session);
            ssh_free(session);
            return;
        }

        Q_EMIT open_term(QVariant::fromValue(new Pty({session, channel})));
    });
}