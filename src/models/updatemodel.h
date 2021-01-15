// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QAbstractListModel>

class Update : public QObject
{
    Q_OBJECT;

public:
    Update(QObject* parent = nullptr) : QObject(parent) {}

    Q_PROPERTY(QString name        MEMBER m_name        CONSTANT);
    Q_PROPERTY(QString old_version MEMBER m_old_version CONSTANT);
    Q_PROPERTY(QString new_version MEMBER m_new_version CONSTANT);

    QString m_name       ;
    QString m_old_version;
    QString m_new_version;
};

using UpdateList = QList<QObject*>;

class Server;

class UpdateModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QObject* server MEMBER m_server WRITE set_server NOTIFY changed_updates);

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
    Q_INVOKABLE [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOT:
    void set_server (QObject* updates);
    void set_updates(QList<QObject*> updates);

Q_SIGNALS:
    void changed_updates();

private:
    Server* m_server;
    QMetaObject::Connection connection;
    int pre_size;
};