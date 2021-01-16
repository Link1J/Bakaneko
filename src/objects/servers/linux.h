// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "../server.h"

class LinuxComputer : public Server
{
    Q_OBJECT;

public:
    LinuxComputer(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QObject* parent = nullptr);

private:

public:

public Q_SLOTS:
    QString get_kernal_type() override;

    void check_for_updates() override;
    void collect_info     () override;

    void shutdown() override;
    void reboot  () override;

Q_SIGNALS:

private:
};
