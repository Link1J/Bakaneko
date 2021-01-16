// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>

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

using UpdateList = QList<Update*>;