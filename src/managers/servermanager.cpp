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
    
    std::lock_guard __lock_guard{server_list_lock};
    
    if (model != nullptr)
    {
        model->beginRowAppend();
    }

    auto temp = Server::create("", "", ip, username, password, "");

    QSettings settings;

    int size = settings.beginReadArray("servers");
    settings.endArray();

    settings.beginWriteArray("servers");
    settings.setArrayIndex(size);
    
    settings.setValue("username", username);
    settings.setValue("password", password);

    settings.setValue("hostname"   , temp->get_hostname   ());
    settings.setValue("mac"        , temp->get_mac        ());
    settings.setValue("ip"         , temp->get_ip         ());
    settings.setValue("kernal_type", temp->get_kernal_type());

    settings.endArray();
    
    servers.push_back(std::unique_ptr<Server>(temp));

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
        servers.push_back(std::unique_ptr<Server>(Server::create(
            settings.value("hostname"   ).toString(),
            settings.value("mac"        ).toString(),
            settings.value("ip"         ).toString(),
            settings.value("username"   ).toString(),
            settings.value("password"   ).toString(),
            settings.value("kernal_type").toString()
        )));

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

        settings.setValue("username", servers[a]->username);
        settings.setValue("password", servers[a]->password);

        settings.setValue("hostname"   , servers[a]->get_hostname   ());
        settings.setValue("mac"        , servers[a]->get_mac        ());
        settings.setValue("ip"         , servers[a]->get_ip         ());
        settings.setValue("kernal_type", servers[a]->get_kernal_type());
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