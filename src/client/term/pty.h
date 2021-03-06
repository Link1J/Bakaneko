// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QSize>
#include <QThread>
#include <QTimer>

#include <libssh/libssh.h>

#include <ljh/expected.hpp>

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

class Pty : public QObject
{
    Q_OBJECT

public:
    Pty(ssh_connection&& server, QObject* parent = nullptr);
    ~Pty();

    void start(const char* term, QSize size, bool auto_read = true);

    std::string read_stdout(bool blocking = true, unsigned char bytes = 255);
    std::string read_stderr(bool blocking = true, unsigned char bytes = 255);

    ssh_connection&& move_connection();

public Q_SLOTS:
    Q_INVOKABLE void send_signal(int signal);
    Q_INVOKABLE void send_data(QString data);

    Q_INVOKABLE void set_size(QSize size);

    void check_for_data();

Q_SIGNALS:
    void receved_signal(int signal);
    void receved_data(QString data);

private:
    std::string read(bool std_err, bool blocking, unsigned char bytes);

    ssh_connection connection;
    QSize size;
    QTimer* data_check;
};
