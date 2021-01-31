// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QAbstractTableModel>

#include "updates.pb.h"

class Server;

class UpdateModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY changed_count);

public:
    enum ServerRoles {
        NameRole = Qt::UserRole + 1,
        OldVersionRole,
        NewVersionRole,
    };
    Q_ENUM(ServerRoles)

    explicit UpdateModel(QObject* parent = nullptr);
    ~UpdateModel() override;

public Q_SLOT:
    std::string data(int index, int role = Qt::DisplayRole) const;
    bool setData(int index, std::string value, int role = Qt::EditRole);

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

Q_SIGNALS:
    void changed_count();

private:
    std::vector<Bakaneko::Update> updates;
};

using UpdateModelPointer = UpdateModel*;
Q_DECLARE_METATYPE(UpdateModelPointer);