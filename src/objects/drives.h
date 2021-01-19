// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>

class Parition;

class Drive : public QObject
{
    Q_OBJECT;

public:
    Drive(QObject* parent = nullptr) : QObject(parent) {}

    Q_PROPERTY(QString          dev_node  MEMBER m_dev_node  CONSTANT);
    Q_PROPERTY(QList<Parition*> paritions MEMBER m_paritions CONSTANT);
    Q_PROPERTY(QString          size      MEMBER m_size      CONSTANT);
    Q_PROPERTY(QString          model     MEMBER m_model     CONSTANT);

    QString          m_dev_node ;
    QList<Parition*> m_paritions;
    QString          m_size     ;
    QString          m_model    ;
};

class Parition : public QObject
{
    Q_OBJECT;

public:
    Parition(QObject* parent = nullptr) : QObject(parent) {}

    Q_PROPERTY(QString dev_node   MEMBER m_dev_node   CONSTANT);
    Q_PROPERTY(QString size       MEMBER m_size       CONSTANT);
    Q_PROPERTY(QString used       MEMBER m_used       CONSTANT);
    Q_PROPERTY(QString mountpoint MEMBER m_mountpoint CONSTANT);
    Q_PROPERTY(QString filesystem MEMBER m_filesystem CONSTANT);

    QString m_dev_node  ;
    QString m_size      ;
    QString m_used      ;
    QString m_mountpoint;
    QString m_filesystem;
};
