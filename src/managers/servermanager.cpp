// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#include "servermanager.h"

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

    QSettings settings;
    int size = settings.beginReadArray("servers");
    settings.endArray();
    settings.beginWriteArray("servers");
    settings.setArrayIndex(size);
    servers.push_back(std::make_unique<Server>());
    settings.setValue("hostname", servers[size]->hostname    = QString::fromStdString(hostname));
    settings.setValue("mac"     , servers[size]->mac_address = QString::fromStdString(MAC     ));
    settings.setValue("ip"      , servers[size]->ip_address  = QString::fromStdString(ip      ));
    settings.setValue("username", servers[size]->username    = QString::fromStdString(username));
    settings.setValue("password", servers[size]->password    = QString::fromStdString(password));
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
    Reload();
}

ServerManager& ServerManager::Instance()
{
    static ServerManager instance;
    return instance;
}

void ServerManager::Reload()
{
    QSettings settings;
    int size = settings.beginReadArray("servers");
    servers.clear();
    servers.reserve(size);
    for (int a = 0; a < size; a++)
    {
        settings.setArrayIndex(a);
        servers.push_back(std::make_unique<Server>());

        servers[a]->hostname    = settings.value("hostname").toString();
        servers[a]->mac_address = settings.value("mac"     ).toString();
        servers[a]->ip_address  = settings.value("ip"      ).toString();
        servers[a]->username    = settings.value("username").toString();
        servers[a]->password    = settings.value("password").toString();

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

    for (int a = 0; a < servers.size(); a++)
    {
        connect(servers[a].get(), &Server::changed_state, model, [=]() { model->update(a); });
    }
}

void ServerManager::RemoveServer(int index)
{
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