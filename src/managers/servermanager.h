// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "objects/server.h"
#include "models/serverlistmodel.h"

#include <QObject>
#include <string>
#include <vector>
#include <memory>

class ServerManager : public QObject
{
    Q_OBJECT

    std::vector<std::unique_ptr<Server>> servers;
    ServerListModel* model = nullptr;

    ServerManager();

    void Reload();

public:
    static ServerManager& Instance();

    size_t size() const;
    Server& operator[](size_t index);
    void NewServer(std::string ip, std::string username, std::string password);

    int GetIndex(Server* server);

    Q_INVOKABLE void AddServer(QString ip, QString username, QString password);
    Q_INVOKABLE void RemoveServer(int index);

    void setModel(ServerListModel* in_model);
    ServerListModel* GetModel();
};

using ServerManagerPointer = ServerManager*;
Q_DECLARE_METATYPE(ServerManagerPointer);