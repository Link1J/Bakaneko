// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QSize>
#include <QThread>

#include <libssh/libssh.h>

class Server;

class ssh_connection
{
    ssh_session session = nullptr;
    ssh_channel channel = nullptr;

public:
    ssh_connection() = default;
    ssh_connection(ssh_session session, ssh_channel channel);

    ssh_connection(const ssh_connection&) = delete;
    ssh_connection& operator=(const ssh_connection&) = delete;

    ssh_connection(ssh_connection&& other);
    ssh_connection& operator=(ssh_connection&& other);

    ~ssh_connection();

    operator ssh_session();
    operator ssh_channel();
};

class Pty;

class PtyReaderThread : public QThread
{
    Q_OBJECT

    PtyReaderThread(Pty* pty);
    void run() override;

Q_SIGNALS:

private:
    Pty* pty;
};

class Pty : public QObject
{
    friend class PtyReaderThread;

    Q_OBJECT

public:
    Pty(Server* server, const char* term, QObject* parent = nullptr);
    ~Pty();

public Q_SLOTS:
    Q_INVOKABLE void send_signal(int signal);
    Q_INVOKABLE void send_data(QString data);

    Q_INVOKABLE void  set_size(QSize size);
    Q_INVOKABLE QSize get_size(          );

    void check_for_data();

Q_SIGNALS:
    void receved_signal(int signal);
    void receved_data(QString data);

    void changed_size();

private:
    ssh_connection connection;
    QSize size;
    QTimer* data_check;
};
