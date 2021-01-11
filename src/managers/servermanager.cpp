// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "servermanager.h"
#include "settings.h"

#include <QSettings>
#include <libssh/libssh.h>
#include <iostream>

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

    memset(buffer, 0, sizeof(buffer));

    auto session = ssh_new();

    ssh_options_set(session, SSH_OPTIONS_HOST, ip.c_str());
    ssh_options_set(session, SSH_OPTIONS_USER, username.c_str());

    ssh_connect(session);
    if (auto error_code = ssh_userauth_password(session, NULL, password.c_str()); error_code != SSH_AUTH_SUCCESS)
    {
        std::cerr << ssh_get_error(session) << "\n";
        return;
    }

    auto channel = ssh_channel_new(session);
    ssh_channel_open_session(channel);

    while ((code = ssh_channel_request_exec(channel, "cat /etc/hostname && cat /sys/class/net/*/address")) == SSH_AGAIN);
    if (code == SSH_OK)
    {
        while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), false)) > 0)
        {
            network_info += buffer;
        }
    }
    else
    {
        std::cerr << ssh_get_error(session) << "\n";
        throw 1;
    }
    
    auto line = network_info.find('\n');
    hostname = network_info.substr(0, line);
    line++;
    MAC = network_info.substr(line, network_info.find('\n', line) - line);

    auto q_hostname = QString::fromStdString(hostname);
    auto q_MAC      = QString::fromStdString(MAC     );
    auto q_ip       = QString::fromStdString(ip      );
    auto q_username = QString::fromStdString(username);
    auto q_password = QString::fromStdString(password);

    QSettings settings;
    int size = settings.beginReadArray("servers");
    settings.endArray();
    settings.beginWriteArray("servers");
    settings.setArrayIndex(size);
    servers.push_back(std::make_unique<Server>(q_hostname, q_MAC, q_ip, q_username, q_password));
    settings.setValue("hostname", q_hostname);
    settings.setValue("mac"     , q_MAC     );
    settings.setValue("ip"      , q_ip      );
    settings.setValue("username", q_username);
    settings.setValue("password", q_password);
    settings.endArray();

    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    
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
    connect(&Settings::Instance(), &Settings::changed_server_refresh_rate, [this](){
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
            settings.value("hostname").toString(),
            settings.value("mac"     ).toString(),
            settings.value("ip"      ).toString(),
            settings.value("username").toString(),
            settings.value("password").toString()
        ));

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
        settings.setValue("hostname", servers[a]->hostname   );
        settings.setValue("mac"     , servers[a]->mac_address);
        settings.setValue("ip"      , servers[a]->ip_address );
        settings.setValue("username", servers[a]->username   );
        settings.setValue("password", servers[a]->password   );
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

void ServerManager::update_server_info()
{
    std::lock_guard __lock_guard{server_list_lock};

    for (int a = 0; a < size(); a++) {
        servers[a]->update_info();
    }
}