// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>

#undef interface
#include "services.pb.h"

class ServiceTypeList : public QAbstractListModel
{
    Q_OBJECT

    std::vector<std::string> items;

public:
    explicit ServiceTypeList(QObject* parent = nullptr);
    ~ServiceTypeList() override;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void Clear();
    void AddItem(std::string item);

public Q_SLOT:
};

class ServiceModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ServerRoles {
        ROLE_state = Qt::UserRole + 1,
        ROLE_enable,
        ROLE_id,
    };
    Q_ENUM(ServerRoles);

    explicit ServiceModel(QObject* parent = nullptr);
    ~ServiceModel() override;

public Q_SLOT:
    Bakaneko::Service& data(int a);

    void flag(int a, std::vector<int> columns);

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Q_INVOKABLE int columnMinWidth(int column);

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

Q_SIGNALS:

private:
    struct MinSize
    {
        size_t index = -1;
        qreal  size  =  0;
    };

    std::vector<Bakaneko::Service> updates;
    std::vector<MinSize> min_width;
};

using ServiceModelPointer = ServiceModel*;
Q_DECLARE_METATYPE(ServiceModelPointer);
