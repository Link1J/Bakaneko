// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "updatemodel.h"

#include <iostream>

#include <QSettings>

#include <objects/server.h>

UpdateModel::UpdateModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

UpdateModel::~UpdateModel() = default;

QVariant UpdateModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto temp = m_server->get_updates()[index.row()];
    if (temp != nullptr)
    {
        if (role == NameRole)
            return QVariant::fromValue(temp->m_name);
        else if (role == OldVersionRole)
            return QVariant::fromValue(temp->m_old_version);
        else if (role == NewVersionRole)
            return QVariant::fromValue(temp->m_new_version);
    }
    return QVariant::fromValue(temp);
}

int UpdateModel::rowCount(const QModelIndex& parent) const
{
    return (int)m_server->get_updates().size();
}

QHash<int, QByteArray> UpdateModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[OldVersionRole] = "old_version";
    roles[NewVersionRole] = "new_version";
    return roles;
}

void UpdateModel::set_server(QObject* updates)
{
    if (m_server)
        disconnect(connection);

    m_server = dynamic_cast<Server*>(updates);

    if (m_server)
    {
        connection = connect(m_server, &Server::new_updates, this, &UpdateModel::set_updates, Qt::UniqueConnection);
        set_updates(m_server->get_updates());
    }
}

void UpdateModel::set_updates(UpdateList updates)
{
    QModelIndex index1 = createIndex(0, 0, nullptr);
    QModelIndex index2 = createIndex(std::min(updates.size(), pre_size), 0, nullptr);
    QVector<int> roles = { NameRole, OldVersionRole, NewVersionRole, };
    
    if (updates.size() > pre_size)
    {
        beginInsertRows(QModelIndex(), pre_size, updates.size());
        endInsertRows();
        Q_EMIT changed_count();
    }
    else if (updates.size() < pre_size)
    {
        beginRemoveRows(QModelIndex(), updates.size(), pre_size);
        endRemoveRows();
        Q_EMIT changed_count();
    }
    
    Q_EMIT dataChanged(index1, index2, roles);

    pre_size = updates.size();
}
