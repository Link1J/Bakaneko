// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QAbstractListModel>

#undef interface
#include "network.hpp"
class AdapterModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY changed_count);

public:
    enum ServerRoles {
        ROLE_name  = Qt::UserRole + 1,
        ROLE_link_speed,
        ROLE_state,
        ROLE_mtu,
        ROLE_mac_address,
        ROLE_ip_address,
        ROLE_rx_rate,
        ROLE_tx_rate,
    };
    Q_ENUM(ServerRoles)

    explicit AdapterModel(QObject* parent = nullptr);
    ~AdapterModel() override;

public Q_SLOT:
    Bakaneko::Adapter& data(int a);
    Bakaneko::Adapter& prev(int a);

    void flag(int a, std::vector<int> roles);

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

Q_SIGNALS:
    void changed_count();

private:
    std::vector<Bakaneko::Adapter> updates;
    std::vector<Bakaneko::Adapter> prevous;
};

using AdapterModelPointer = AdapterModel*;
Q_DECLARE_METATYPE(AdapterModelPointer);