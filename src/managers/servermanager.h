// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "objects/server.h"
#include "models/serverlistmodel.h"

#include <QObject>
#include <QTimer>

#include <string>
#include <vector>
#include <memory>
#include <mutex>

class ServerManager : public QObject
{
    Q_OBJECT

    std::vector<std::unique_ptr<Server>> servers;
    std::shared_ptr<QTimer> timer;
    std::mutex server_list_lock;

    ServerListModel* model = nullptr;

    ServerManager();
    ~ServerManager();

    void Reload();

public:
    static ServerManager& Instance();

    void start();
    
    size_t size() const;
    Server& operator[](size_t index);

    int GetIndex(Server* server);

    void setModel(ServerListModel* in_model);
    ServerListModel* GetModel();

public Q_SLOTS:
    Q_INVOKABLE void AddServer(QString ip, QString username, QString password);
    Q_INVOKABLE void RemoveServer(int index);

    Q_INVOKABLE void server_offline(Server* server);
    Q_INVOKABLE void server_online (Server* server);
    
    void update_server_info(bool wait = false);
};

using ServerManagerPointer = ServerManager*;
Q_DECLARE_METATYPE(ServerManagerPointer);