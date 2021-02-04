// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QTimer>

class LoginData : public QObject
{
    Q_OBJECT;

public:
    Q_PROPERTY(QObject* page     MEMBER page             );
    Q_PROPERTY(QObject* login    MEMBER login    REQUIRED);
    Q_PROPERTY(QString  username MEMBER username REQUIRED);
    Q_PROPERTY(QString  password MEMBER password REQUIRED);

    QObject* page    ;
    QObject* login   ;
    QString  username;
    QString  password;
};