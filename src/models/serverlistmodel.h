// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QAbstractListModel>

class ServerListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ServerRoles {
        NameRole = Qt::UserRole + 1,
        StateRole,
        DataRole,
        IPRole,
    };
    Q_ENUM(ServerRoles)

    explicit ServerListModel(QObject* parent = nullptr);
    ~ServerListModel() override;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOT:
    void update(int index);
    void beginRowAppend();
    void endRowAppend();
    void beginRowRemove(int index);
    void endRowRemove();
};