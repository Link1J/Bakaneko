// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "adaptermodel.h"

#include <objects/server.h>

AdapterModel::AdapterModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

AdapterModel::~AdapterModel() = default;

Bakaneko::Adapter& AdapterModel::data(int a)
{
    return updates[a];
}

Bakaneko::Adapter& AdapterModel::prev(int a)
{
    return prevous[a];
}

void AdapterModel::flag(int a, std::vector<int> roles)
{
    Q_EMIT dataChanged(createIndex(a, 0), createIndex(a, 0), QVector<int>(roles.begin(), roles.end()));
}

QVariant AdapterModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto& temp = updates[index.row()];

    if (role == ROLE_name       ) return QVariant::fromValue(QString::fromStdString(temp.name       ()));
    if (role == ROLE_mtu        ) return QVariant::fromValue(                       temp.mtu        () );
    if (role == ROLE_mac_address) return QVariant::fromValue(QString::fromStdString(temp.mac_address()));
    if (role == ROLE_ip_address ) return QVariant::fromValue(QString::fromStdString(temp.ip_address ()));

    if (role == ROLE_link_speed )
    {
        if (temp.link_speed() == 0)
            return QVariant::fromValue(QString::fromStdString("Unknown"));
        return QVariant::fromValue(QString::fromStdString(bitrate_to_string(temp.link_speed())));
    }

    if (role == ROLE_state)
    {
        return QVariant::fromValue(QString::fromStdString(temp.state() == Bakaneko::Adapter_State_Up ? "Up" : "Down"));
    }

    if (role == ROLE_rx_rate)
    {
        auto delta_time = std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::nanoseconds(temp.time()) - std::chrono::nanoseconds(prevous[index.row()].time())
        );
        auto delta_bytes = (temp.bytes_rx() - prevous[index.row()].bytes_rx()) * 8;
        return QVariant::fromValue(QString::fromStdString(bitrate_to_string(delta_bytes / delta_time.count())));
    }

    if (role == ROLE_tx_rate)
    {
        auto delta_time = std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::nanoseconds(temp.time()) - std::chrono::nanoseconds(prevous[index.row()].time())
        );
        auto delta_bytes = (temp.bytes_tx() - prevous[index.row()].bytes_tx()) * 8;
        return QVariant::fromValue(QString::fromStdString(bitrate_to_string(delta_bytes / delta_time.count())));
    }

    return QVariant();
}

int AdapterModel::rowCount(const QModelIndex& parent) const
{
    return (int)updates.size();
}

QHash<int, QByteArray> AdapterModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ROLE_name       ] = "name";
    roles[ROLE_state      ] = "connection_state";
    roles[ROLE_link_speed ] = "link_speed";
    roles[ROLE_mtu        ] = "mtu";
    roles[ROLE_mac_address] = "mac_address";
    roles[ROLE_ip_address ] = "ip_address";
    roles[ROLE_rx_rate    ] = "rx_rate";
    roles[ROLE_tx_rate    ] = "tx_rate";
    return roles;
}

bool AdapterModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginInsertRows(parent, row, row + count);
    updates.insert(updates.begin() + row, count, Bakaneko::Adapter{});
    prevous.insert(prevous.begin() + row, count, Bakaneko::Adapter{});
    Q_EMIT endInsertRows();
    Q_EMIT changed_count();
    return true;
}

bool AdapterModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginRemoveRows(parent, row, row + count);
    updates.erase(updates.begin() + row, updates.begin() + row + count);
    prevous.erase(prevous.begin() + row, prevous.begin() + row + count);
    Q_EMIT endRemoveRows();
    Q_EMIT changed_count();
    return true;
}