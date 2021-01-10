// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QSize>

#include <libssh/libssh.h>

class Server;

class ssh_connection
{
    ssh_session session = nullptr;
    ssh_channel channel = nullptr;

public:
    ssh_connection() = default;

    ssh_connection(ssh_session session, ssh_channel channel)
        : session(session), channel(channel)
    {}

    ssh_connection(const ssh_connection&) = delete;
    ssh_connection& operator=(const ssh_connection&) = delete;

    ssh_connection(ssh_connection&& other)
        : session(other.session), channel(other.channel)
    {
        other.session = nullptr;
        other.channel = nullptr;
    }

    ssh_connection& operator=(ssh_connection&& other)
    {
        session = other.session;
        channel = other.channel;
        other.session = nullptr;
        other.channel = nullptr;
        return *this;
    }

    ~ssh_connection()
    {
        if (channel)
        {
            ssh_channel_close(channel);
            ssh_channel_send_eof(channel);
            ssh_channel_free(channel);
        }
        if (session)
        {
            ssh_disconnect(session);
            ssh_free(session);
        }
    }

    operator ssh_session() { return session; }
    operator ssh_channel() { return channel; }
};

class Pty : public QObject
{
    Q_OBJECT

public:
    Pty(Server* server, QObject* parent = nullptr);
    ~Pty();

public Q_SLOTS:
    Q_INVOKABLE void send_signal(int signal);

    Q_INVOKABLE void  set_size(QSize size);
    Q_INVOKABLE QSize get_size(          );

Q_SIGNALS:
    void receved_signal(int signal);

    void changed_size();

private:
    ssh_connection connection;
    QSize size;
};
