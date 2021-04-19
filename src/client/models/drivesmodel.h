// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QAbstractListModel>
#include <QAbstractTableModel>

#undef interface
#include "drives.hpp"

class PartitionModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY changed_count);

public:
    enum ServerRoles {
        ROLE_dev_node  = Qt::UserRole + 1,
        ROLE_size,
        ROLE_used,
        ROLE_mountpoint,
        ROLE_filesystem,
    };
    Q_ENUM(ServerRoles)

    explicit PartitionModel(QObject* parent = nullptr);
    ~PartitionModel() override;

public Q_SLOT:
    Bakaneko::Partition& data(int a);

    void flag(int a, std::vector<int> roles);

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

Q_SIGNALS:
    void changed_count();

private:
    std::vector<Bakaneko::Partition> updates;
};

using PartitionModelPointer = PartitionModel*;
Q_DECLARE_METATYPE(PartitionModelPointer);

class DrivesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY changed_count);

public:
    enum ServerRoles {
        ROLE_dev_node  = Qt::UserRole + 1,
        ROLE_size,
        ROLE_model,
        ROLE_manufacturer,
        ROLE_interface,
        ROLE_partition,
    };
    Q_ENUM(ServerRoles)

    explicit DrivesModel(QObject* parent = nullptr);
    ~DrivesModel() override;

public Q_SLOT:
    Bakaneko::Drive& data(int a);
    PartitionModel& partition(int a);

    void flag(int a, std::vector<int> roles);

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

Q_SIGNALS:
    void changed_count();

private:
    std::vector<Bakaneko::Drive> updates;
    std::vector<std::shared_ptr<PartitionModel>> partitions;
};

using DrivesModelPointer = DrivesModel*;
Q_DECLARE_METATYPE(DrivesModelPointer);