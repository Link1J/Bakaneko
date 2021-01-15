// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "servermanager.h"
#include "settings.h"

#include <QSettings>
#include <QThread>
#include <QIcon>

#include <KLocalizedString>
#include <KNotification>

#include <libssh/libssh.h>

#include <iostream>
#include <atomic>

void ServerManager::AddServer(QString ip, QString username, QString password)
{
    std::string temp_ip       = ip      .toUtf8().constData();
    std::string temp_username = username.toUtf8().constData();
    std::string temp_password = password.toUtf8().constData();
    NewServer(temp_ip, temp_username, temp_password);
}

void ServerManager::NewServer(std::string ip, std::string username, std::string password)
{
    std::lock_guard __lock_guard{server_list_lock};
    
    if (model != nullptr)
    {
        model->beginRowAppend();
    }

    char buffer[256];
    int nbytes;
    int volatile code;
    std::string hostname;
    std::string MAC;
    std::string network_info;

    auto q_ip       = QString::fromStdString(ip      );
    auto q_username = QString::fromStdString(username);
    auto q_password = QString::fromStdString(password);

    auto temp = std::make_unique<Server>("", "", q_ip, q_username, q_password, "", this);

    int exit_code;
    std::string std_out;
    std::string std_err;

    QString q_kernal_type;
    QString q_hostname   ;
    QString q_MAC        ;

    std::tie(exit_code, std_out, std_err) = temp->run_command("uname");

    if (exit_code != 0)
        q_kernal_type = "Windows";
    else
        q_kernal_type = QString::fromStdString(std_out.substr(0, std_out.size() - 1));

    if (q_kernal_type == "Linux")
    {
        std::tie(exit_code, std_out, std_err) = temp->run_command("cat /etc/hostname");
        q_hostname = QString::fromStdString(std_out.substr(0, std_out.find('\n')));

        std::tie(exit_code, std_out, std_err) = temp->run_command("cat /sys/class/net/*/address");
        q_MAC = QString::fromStdString(std_out.substr(0, std_out.find('\n')));
    }
    else if (q_kernal_type == "Windows")
    {
        std::tie(exit_code, std_out, std_err) = temp->run_command("ipconfig /all");

        auto get_value = [std_out](std::string key) -> std::string {
            auto text_pos = std_out.find(key) + key.length();
            if (text_pos == std::string::npos + key.length())
                return "";
            text_pos = std_out.find(':', text_pos) + 2;
            auto newline_pos = std_out.find('\n', text_pos);
            return std_out.substr(text_pos, newline_pos - text_pos);
        };

        q_hostname = QString::fromStdString(get_value("Host Name"));

        auto temp = get_value("Physical Address");
        std::replace(temp.begin(), temp.end(), '-', ':');
        q_MAC = QString::fromStdString(temp);
    }

    QSettings settings;
    int size = settings.beginReadArray("servers");
    settings.endArray();
    settings.beginWriteArray("servers");
    settings.setArrayIndex(size);
    servers.push_back(std::make_unique<Server>(q_hostname, q_MAC, q_ip, q_username, q_password, q_kernal_type));
    settings.setValue("hostname"   , q_hostname   );
    settings.setValue("mac"        , q_MAC        );
    settings.setValue("ip"         , q_ip         );
    settings.setValue("username"   , q_username   );
    settings.setValue("password"   , q_password   );
    settings.setValue("kernal_type", q_kernal_type);
    settings.endArray();

    connect(servers[size].get(), &Server::this_online , this, &ServerManager::server_online );
    connect(servers[size].get(), &Server::this_offline, this, &ServerManager::server_offline);
    
    if (model != nullptr)
    {
        connect(servers[size].get(), &Server::changed_state, model, [=]() { model->update(size); });
        model->endRowAppend();
    }
}

ServerManager::ServerManager()
{
    timer = std::make_shared<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, &ServerManager::update_server_info);
    connect(&Settings::Instance(), &Settings::changed_server_refresh_rate, this, [this](){
        this->timer->start(Settings::Instance().get_server_refresh_rate() * 1000);
    });
    timer->start(Settings::Instance().get_server_refresh_rate() * 1000);
    Reload();
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
        servers.push_back(std::make_unique<Server>(
            settings.value("hostname"   ).toString(),
            settings.value("mac"        ).toString(),
            settings.value("ip"         ).toString(),
            settings.value("username"   ).toString(),
            settings.value("password"   ).toString(),
            settings.value("kernal_type").toString(),
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
        settings.setValue("hostname"   , servers[a]->hostname   );
        settings.setValue("mac"        , servers[a]->mac_address);
        settings.setValue("ip"         , servers[a]->ip_address );
        settings.setValue("username"   , servers[a]->username   );
        settings.setValue("password"   , servers[a]->password   );
        settings.setValue("kernal_type", servers[a]->kernal_type);
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

void ServerManager::update_server_info()
{
    std::lock_guard __lock_guard{server_list_lock};
    // This spawns a thread for each server. It could slow down a computer because of too many server.
    // If a problem arises, we can solve it then.
    for (int a = 0; a < size(); a++) {
        // Does this leak memory?
        auto thread = QThread::create([this, a](){
            servers[a]->update_info();
            QThread::currentThread()->quit();
        });
        connect(thread, &QThread::started , [](){ active_threads++; });
        connect(thread, &QThread::finished, [](){ active_threads--; });
        thread->start();
    }
}

ServerManager::~ServerManager()
{
    std::lock_guard __lock_guard{server_list_lock};
    while (active_threads > 0);
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