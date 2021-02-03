// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "servermanager.h"
#include "settings.h"

#include <QSettings>
#include <QThread>
#include <QIcon>
#include <QtConcurrent>
#include <QApplication>

#include <KLocalizedString>
#include <KNotification>

#include <libssh/libssh.h>

#include <iostream>
#include <thread>
#include <atomic>

static asio::io_context io_context;

Bakaneko::System get_system_info(asio::ip::tcp::socket& socket, const std::string& ip_address)
{
    Bakaneko::System info;
    boost::system::error_code ec;

    beast::http::request<beast::http::string_body> req{beast::http::verb::get, "/system", 11};

    req.set(beast::http::field::host, ip_address);
    req.set(beast::http::field::content_type, "application/x-protobuf");
    req.version(11);

    beast::http::write(socket, req, ec);
    
    if (ec) return info;

    beast::flat_buffer buffer;
    beast::http::response<beast::http::string_body> res;

    beast::http::read(socket, buffer, res, ec);

    if (ec) return info;
    if (res.result_int() != 200) return info;

    info.ParseFromString(res.body());
    return info;
}

boost::system::error_code check_if_server(asio::ip::tcp::socket& socket, const std::string& ip_address)
{
    boost::system::error_code ec;
    asio::ip::tcp::resolver resolver(io_context);
    auto const results = resolver.resolve(ip_address, "29921");
    asio::connect(socket, results.begin(), results.end(), ec);
    if (ec) return ec;

    beast::http::request<beast::http::string_body> req{beast::http::verb::head, "/", 11};

    req.set(beast::http::field::host, ip_address);
    req.version(11);

    beast::http::write(socket, req, ec);
    if (ec) return ec;

    beast::flat_buffer buffer;
    beast::http::response<beast::http::empty_body> res;

    beast::http::read(socket, buffer, res, ec);
    if (ec) return ec;

    if (res.result_int() != 302)
        ec = make_error_code(boost::system::errc::not_supported);
    return ec;
}

void ServerManager::AddServer(QString ip)
{
    change_server_options(servers.size(), ip);
}

static QThread server_thread;

ServerManager::ServerManager()
{
    Reload();
}

void ServerManager::start()
{
    // this->moveToThread(&server_thread);
    timer = std::make_shared<QTimer>(this);
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(update_server_info()));
    connect(&Settings::Instance(), &Settings::changed_server_refresh_rate, this, [this](){
        this->timer->start(Settings::Instance().get_server_refresh_rate() * 1000);
    });
    timer->start(Settings::Instance().get_server_refresh_rate() * 1000);
}

ServerManager& ServerManager::Instance()
{
    static ServerManager instance;
    return instance;
}

void ServerManager::Reload()
{
    std::lock_guard __lock_guard{server_list_lock};
    
    QSettings settings;
    int size = settings.beginReadArray("servers");
    servers.clear();
    servers.reserve(size);
    for (int a = 0; a < size; a++)
    {
        settings.setArrayIndex(a);
        servers.push_back(std::make_shared<Server>(
            settings.value("hostname").toString().toUtf8().data(),
            settings.value("mac"     ).toString().toUtf8().data(),
            settings.value("ip"      ).toString().toUtf8().data(),
            this
        ));

        connect(servers[a].get(), &Server::this_online , this, &ServerManager::server_online );
        connect(servers[a].get(), &Server::this_offline, this, &ServerManager::server_offline);

        if (model != nullptr)
        {
            connect(servers[a].get(), &Server::changed_state, model, [=]() { model->update(a); });
        }
    }
    settings.endArray();
}

size_t ServerManager::size() const
{
    return servers.size();
}

Server& ServerManager::operator[](size_t index)
{
    return *servers[index];
}

void ServerManager::setModel(ServerListModel* in_model)
{
    model = in_model;

    std::lock_guard __lock_guard{server_list_lock};
    for (int a = 0; a < servers.size(); a++)
    {
        connect(servers[a].get(), &Server::changed_state, model, [=]() { model->update(a); });
    }
}

void ServerManager::RemoveServer(int index)
{
    std::lock_guard __lock_guard{server_list_lock};

    if (model != nullptr)
    {
        model->beginRowRemove(index);
    }

    servers.erase(servers.begin() + index);

    QSettings settings;
    settings.beginWriteArray("servers", (int)servers.size());
    for (int a = 0; a < servers.size(); a++)
    {
        settings.setArrayIndex(a);

        settings.setValue("hostname", servers[a]->get_hostname());
        settings.setValue("mac"     , servers[a]->get_mac     ());
        settings.setValue("ip"      , servers[a]->get_ip      ());
    }
    settings.endArray();

    if (model != nullptr)
    {
        model->endRowRemove();
    }
}

int ServerManager::GetIndex(Server* server)
{
    for (int a = 0; a < servers.size(); a++)
    {
        if (servers[a].get() == server)
            return a;
    }
    return -1;
}

ServerListModel* ServerManager::GetModel()
{
    return model;
}

static std::atomic<size_t> active_threads;

void ServerManager::update_server_info(bool wait)
{
    using namespace std::chrono_literals;
    std::lock_guard __lock_guard{server_list_lock};

    for (int a = 0; a < size(); a++)
    {
        QtConcurrent::run([this, a]{
            servers[a]->update_info();
        });
    }

    if (wait)
    {
        for (int a = 0; a < size(); a++)
        {
            while (servers[a]->steps_done.try_wait())
                qApp->processEvents();
        }
    }
}

ServerManager::~ServerManager()
{
}

void ServerManager::change_server_options(int index, QString ip)
{
    std::lock_guard __lock_guard{server_list_lock};

    std::string ip_address = ip.toUtf8().data();

    asio::ip::tcp::socket socket(io_context);
    auto ec = check_if_server(socket, ip_address);
    if (ec)
    {
        std::cerr << ec.message() << std::endl;
        return;
    }

    auto system_info = get_system_info(socket, ip_address);

    if (system_info.hostname().empty() || system_info.mac_address().empty() || system_info.ip_address().empty())
        return;
    
    if (model != nullptr && index == servers.size())
    {
        model->beginRowAppend();
    }

    auto temp = std::make_shared<Server>(system_info.hostname(), system_info.mac_address(), ip_address, this);

    connect(temp.get(), &Server::this_online , this, &ServerManager::server_online );
    connect(temp.get(), &Server::this_offline, this, &ServerManager::server_offline);
    if (model != nullptr)
        connect(temp.get(), &Server::changed_state, model, [=]() { model->update(index); });
    

    if (model != nullptr && index == servers.size())
    {
        model->endRowAppend();
    }

    if (index == servers.size())
        servers.push_back(temp);
    else
        servers[index] = temp;

    QSettings settings;

    settings.beginWriteArray("servers", servers.size());
    settings.setArrayIndex(index);

    settings.setValue("hostname", temp->get_hostname());
    settings.setValue("mac"     , temp->get_mac     ());
    settings.setValue("ip"      , temp->get_ip      ());

    settings.endArray();
}

void ServerManager::server_offline(Server* server)
{
    KNotification::event(
        "server_offline",
        "Server Offline",
        i18n("%1 has gone offline", server->get_hostname()),
        QIcon::fromTheme("offline").pixmap(32)
    );
}

void ServerManager::server_online(Server* server)
{
    KNotification::event(
        "server_online",
        "Server Online",
        i18n("%1 has come online", server->get_hostname()),
        QIcon::fromTheme("online").pixmap(32)
    );
}