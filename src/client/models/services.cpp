// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "services.h"
#include <PlatformTheme>
#include <QFontMetricsF>
#include <algorithm>

ServiceTypeList::ServiceTypeList(QObject* parent)
    : QAbstractListModel(parent)
{
}

ServiceTypeList::~ServiceTypeList() = default;

QVariant ServiceTypeList::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    return QString::fromStdString(items[index.row()]);
}

int ServiceTypeList::rowCount(const QModelIndex& parent) const
{
    return (int)items.size();
}

void ServiceTypeList::Clear()
{
    beginRemoveRows(QModelIndex{}, 0, items.size());
    items.clear();
    endRemoveRows();
}

void ServiceTypeList::AddItem(std::string item)
{
    beginInsertRows(QModelIndex{}, items.size(), items.size() + 1);
    items.push_back(item);
    endInsertRows();
}

QHash<int, QByteArray> ServiceTypeList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    return roles;
}

ServiceModel::ServiceModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    min_width.resize(columnCount());

    QFontMetrics metrics{Kirigami::PlatformTheme().defaultFont()};
    int largeSpacing = int(metrics.height()) / 4 * 2;
    for (int a = 0; a < columnCount(); a++)
    {
        min_width[a].size = metrics.boundingRect(headerData(a, Qt::Horizontal).toString()).width() + largeSpacing * 2;
    }
}

ServiceModel::~ServiceModel() = default;

Bakaneko::Service& ServiceModel::data(int a)
{
    return updates[a];
}

void ServiceModel::flag(int row, std::vector<int> columns)
{
    QFontMetrics metrics{Kirigami::PlatformTheme().defaultFont()};
    int largeSpacing = int(metrics.height()) / 4 * 2;

    for (auto& column : columns)
    {
        qreal width = metrics.boundingRect(data(createIndex(row, column)).toString()).width() + largeSpacing * 2;
        if (width > min_width[column].size)
        {
            min_width[column].size = width;
            min_width[column].index = row;
        }
        else if (row == min_width[column].index && width < min_width[column].size)
        {
            auto width = metrics.boundingRect(headerData(column, Qt::Horizontal).toString()).width() + largeSpacing * 2;
            if (width > min_width[column].size)
            {
                min_width[column].size  = width;
                min_width[column].index = -1;
            }

            for (int a = 0; a < rowCount(); a++)
            {
                if (a == row)
                    continue;

                width = metrics.boundingRect(data(createIndex(a, column)).toString()).width() + largeSpacing * 2;
                if (width > min_width[column].size)
                {
                    min_width[column].size = width;
                    min_width[column].index = a;
                }
            }
        }

        Q_EMIT dataChanged(createIndex(row, column), createIndex(row, column), QVector<int>(Qt::DisplayRole));
    }
}

QVariant ServiceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto row = index.row();
    auto& temp = updates[row];

    if (role == Qt::ToolTipRole)
    {
        return QString::fromStdString(temp.description());
    }
    if (role == ROLE_state)
    {
        return temp.state();
    }
    if (role == ROLE_enable)
    {
        return temp.enabled();
    }
    if (role == ROLE_id)
    {
        return QString::fromStdString(temp.id());
    }

    switch (index.column())
    {
    case 0:
        switch (temp.state())
        {
        case Bakaneko::Service_State_Running:
            return "Running";
        case Bakaneko::Service_State_Starting:
            return "Starting";
        case Bakaneko::Service_State_Stopped:
            return "Stopped";
        case Bakaneko::Service_State_Stopping:
            return "Stopping";
        default:
            return "Unknown";
        }

    case 1:
        return temp.enabled() ? "Enabled" : "Disabled";

    case 2:
        return QString::fromStdString(temp.type());

    case 3:
        return QString::fromStdString(temp.display_name());
    }
    return QVariant();
}

int ServiceModel::rowCount(const QModelIndex& parent) const
{
    return (int)updates.size();
}

int ServiceModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}

QHash<int, QByteArray> ServiceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::ToolTipRole] = "tooltip";
    roles[ROLE_state     ] = "service_state";
    roles[ROLE_enable    ] = "service_enabled";
    roles[ROLE_id        ] = "service_id";
    return roles;
}

QVariant ServiceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return section + 1;

    switch (section)
    {
    case 0: return "State";
    case 1: return "Enabled";
    case 2: return "Type";
    case 3: return "Name";
    }
    return QVariant{};
}

bool ServiceModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, row + count);
    updates.insert(updates.begin() + row, count, Bakaneko::Service{});
    endInsertRows();
    return true;
}

bool ServiceModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count);
    updates.erase(updates.begin() + row, updates.begin() + row + count);
    endRemoveRows();
    return true;
}

int ServiceModel::columnMinWidth(int column)
{
    return min_width[column].size;
}

bool ServiceModel::fuzzy_check(int row, QString data)
{
    std::string search = data.toUtf8().data();
    std::string text = updates[row].display_name();

    std::transform(search.begin(), search.end(), search.begin(), &toupper);
    std::transform(text  .begin(), text  .end(), text  .begin(), &toupper);

    return text.find(search) != std::string::npos;
}