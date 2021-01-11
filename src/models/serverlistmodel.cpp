// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "serverlistmodel.h"

#include <iostream>

#include <QSettings>

#include "objects/server.h"
#include "managers/servermanager.h"

ServerListModel::ServerListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    ServerManager::Instance().setModel(this);
}

ServerListModel::~ServerListModel() = default;

QVariant ServerListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    Server& temp = ServerManager::Instance()[index.row()];
    if (role == NameRole)
    {
        return QVariant(temp.get_hostname());
    }
    else if (role == StateRole)
    {
        return QVariant::fromValue(temp.get_state());
    }
    else if (role == IPRole)
    {
        return QVariant::fromValue(temp.get_ip());
    }
    return QVariant::fromValue<Server*>(&temp);
}

int ServerListModel::rowCount(const QModelIndex& parent) const
{
    return (int)ServerManager::Instance().size();
}

QHash<int, QByteArray> ServerListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "hostname";
    roles[StateRole] = "state";
    roles[DataRole] = "data";
    roles[IPRole] = "ip";
    return roles;
}

void ServerListModel::update(int index)
{
    QVector<int> roles = {
        StateRole,
        NameRole
    };

    int size = (int)ServerManager::Instance().size();

    QModelIndex index1, index2;
    index1 = createIndex(index, 0, nullptr);
    index2 = createIndex(index, 0, nullptr);
    Q_EMIT dataChanged(index1, index2, roles);
}

void ServerListModel::beginRowAppend()
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
}

void ServerListModel::endRowAppend()
{
    endInsertRows();
}

void ServerListModel::beginRowRemove(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
}

void ServerListModel::endRowRemove()
{
    endRemoveRows();
}