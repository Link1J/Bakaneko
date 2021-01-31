// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "drivesmodel.h"

#include <iostream>

#include <QSettings>

#include <objects/server.h>

DrivesModel::DrivesModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

DrivesModel::~DrivesModel() = default;


Bakaneko::Drive& DrivesModel::data(int a)
{
    return updates[a];
}

PartitionModel& DrivesModel::partition(int a)
{
    return *(partitions[a].get());
}

void DrivesModel::flag(int a, std::vector<int> roles)
{
    Q_EMIT dataChanged(createIndex(a, 0), createIndex(a, 0), QVector<int>(roles.begin(), roles.end()));
}

QVariant DrivesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto& temp = updates[index.row()];

    if (role == ROLE_dev_node    ) return QVariant::fromValue(QString::fromStdString(                      temp.dev_node    () ));
    if (role == ROLE_size        ) return QVariant::fromValue(QString::fromStdString(bytes_to_string<1000>(temp.size        ())));
    if (role == ROLE_model       ) return QVariant::fromValue(QString::fromStdString(                      temp.model       () ));
    if (role == ROLE_manufacturer) return QVariant::fromValue(QString::fromStdString(                      temp.manufacturer() ));
    if (role == ROLE_interface   ) return QVariant::fromValue(QString::fromStdString(                      temp.interface   () ));
    if (role == ROLE_partition   ) return QVariant::fromValue(partitions[index.row()].get());

    return QVariant();
}

int DrivesModel::rowCount(const QModelIndex& parent) const
{
    return (int)updates.size();
}

QHash<int, QByteArray> DrivesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ROLE_dev_node    ] = "dev_node";
    roles[ROLE_size        ] = "size";
    roles[ROLE_model       ] = "model";
    roles[ROLE_manufacturer] = "manufacturer";
    roles[ROLE_interface   ] = "interface";
    roles[ROLE_partition   ] = "partitions";
    return roles;
}

bool DrivesModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginInsertRows(parent, row, row + count);
    updates.insert(updates.begin() + row, count, Bakaneko::Drive{});
    partitions.insert(partitions.begin() + row, count, nullptr);
    for (int a = row; a < row + count; a++)
        partitions[a] = std::make_shared<PartitionModel>(this);
    Q_EMIT endInsertRows();
    Q_EMIT changed_count();
    return true;
}

bool DrivesModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginRemoveRows(parent, row, row + count);
    updates.erase(updates.begin() + row, updates.begin() + row + count);
    partitions.erase(partitions.begin() + row, partitions.begin() + row + count);
    Q_EMIT endRemoveRows();
    Q_EMIT changed_count();
    return true;
}

PartitionModel::PartitionModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

PartitionModel::~PartitionModel() = default;

Bakaneko::Partition& PartitionModel::data(int a)
{
    return updates[a];
}

void PartitionModel::flag(int a, std::vector<int> roles)
{
    Q_EMIT dataChanged(createIndex(a, 0), createIndex(a, columnCount()), QVector<int>(roles.begin(), roles.end()));
}

QVariant PartitionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0: role = ROLE_dev_node  ; break;
        case 1: role = ROLE_filesystem; break;
        case 2: role = ROLE_mountpoint; break;
        case 3: role = ROLE_size      ; break;
        case 4: role = ROLE_used      ; break;
        }
        return data(index, role);
    }

    auto& temp = updates[index.row()];

    if (role == ROLE_dev_node  ) return QVariant::fromValue(QString::fromStdString(                      temp.dev_node  ()                              )      );
    if (role == ROLE_size      ) return QVariant::fromValue(QString::fromStdString(bytes_to_string<1000>(temp.size      ()                      )       )      );
    if (role == ROLE_used      ) return QVariant::fromValue(QString::number       (               (int)((temp.used      () / (double)temp.size()) * 100)) + "%");
    if (role == ROLE_mountpoint) return QVariant::fromValue(QString::fromStdString(                      temp.mountpoint()                              )      );
    if (role == ROLE_filesystem) return QVariant::fromValue(QString::fromStdString(                      temp.filesystem()                              )      );

    return QVariant();
}

int PartitionModel::rowCount(const QModelIndex& parent) const
{
    return (int)updates.size();
}

QHash<int, QByteArray> PartitionModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ROLE_dev_node  ] = "dev_node";
    roles[ROLE_size      ] = "size";
    roles[ROLE_used      ] = "used";
    roles[ROLE_mountpoint] = "mountpoint";
    roles[ROLE_filesystem] = "filesystem";
    return roles;
}

bool PartitionModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginInsertRows(parent, row, row + count);
    updates.insert(updates.begin() + row, count, Bakaneko::Partition{});
    Q_EMIT endInsertRows();
    Q_EMIT changed_count();
    return true;
}

bool PartitionModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginRemoveRows(parent, row, row + count);
    updates.erase(updates.begin() + row, updates.begin() + row + count);
    Q_EMIT endRemoveRows();
    Q_EMIT changed_count();
    return true;
}

int PartitionModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant PartitionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return section + 1;

    switch (section)
    {
    case 0: return "Dev Node";
    case 1: return "Filesystem";
    case 2: return "Mountpoint";
    case 3: return "Size";
    case 4: return "Used";
    }
    return QVariant{};
}