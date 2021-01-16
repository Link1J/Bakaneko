// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QAbstractListModel>

#include <objects/update.h>

class Server;

class UpdateModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* server MEMBER m_server WRITE set_server NOTIFY changed_updates);
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

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOT:
    void set_server (QObject* updates);
    void set_updates(QList<Update*> updates);

Q_SIGNALS:
    void changed_updates();
    void changed_count  ();

private:
    Server* m_server;
    QMetaObject::Connection connection;
    int pre_size;
};