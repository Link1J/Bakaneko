// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "updatemodel.h"

#include <iostream>

#include <QSettings>

#include <objects/server.h>

UpdateModel::UpdateModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

UpdateModel::~UpdateModel() = default;

bool UpdateModel::setData(int index, std::string value, int role)
{
    auto& temp = updates[index];
         if (role == NameRole      ) temp.set_name       (value);
    else if (role == OldVersionRole) temp.set_old_version(value);
    else if (role == NewVersionRole) temp.set_new_version(value);
    Q_EMIT dataChanged(createIndex(index, 0), createIndex(index, 0), {role});
    return true;
}

bool UpdateModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;
    
    return setData(index.row(), value.toString().toUtf8().data(), role);
}

std::string UpdateModel::data(int index, int role) const
{
    auto& temp = updates[index];
    if (role == NameRole      ) return temp.name       ();
    if (role == OldVersionRole) return temp.old_version();
    if (role == NewVersionRole) return temp.new_version();
    return "";
}

QVariant UpdateModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
        
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0: role = NameRole      ; break;
        case 1: role = OldVersionRole; break;
        case 2: role = NewVersionRole; break;
        }
        return data(index, role);
    }

    return QVariant::fromValue(QString::fromStdString(data(index.row(), role)));
}

int UpdateModel::rowCount(const QModelIndex& parent) const
{
    return (int)updates.size();
}

int UpdateModel::columnCount(const QModelIndex& parent) const
{
    return 3;
}

QHash<int, QByteArray> UpdateModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole      ] = "name";
    roles[OldVersionRole] = "old_version";
    roles[NewVersionRole] = "new_version";
    return roles;
}

bool UpdateModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginInsertRows(parent, row, row + count);
    Bakaneko::Update temp;
    updates.insert(updates.begin() + row, count, temp);
    Q_EMIT endInsertRows();
    Q_EMIT changed_count();
    return true;
}

bool UpdateModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Q_EMIT beginRemoveRows(parent, row, row + count);
    updates.erase(updates.begin() + row, updates.begin() + row + count);
    Q_EMIT endRemoveRows();
    Q_EMIT changed_count();
    return true;
}

QVariant UpdateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return section + 1;

    switch (section)
    {
    case 0: return "Name";
    case 1: return "Installed Version";
    case 2: return "New Version";
    }
    return QVariant{};
}