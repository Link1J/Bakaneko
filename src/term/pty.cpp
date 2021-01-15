// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "pty.h"
#include "objects/server.h"

#include <string_view>
#include <array>

using namespace std::literals;

ssh_connection::ssh_connection(ssh_session session, ssh_channel channel)
    : session(session), channel(channel)
{}

ssh_connection::ssh_connection(ssh_connection&& other)
    : session(other.session), channel(other.channel)
{
    other.session = nullptr;
    other.channel = nullptr;
}

ssh_connection& ssh_connection::operator=(ssh_connection&& other)
{
    session = other.session;
    channel = other.channel;
    other.session = nullptr;
    other.channel = nullptr;
    return *this;
}

ssh_connection::~ssh_connection()
{
    if (channel)
    {
        ssh_channel_close(channel);
        ssh_channel_send_eof(channel);
        ssh_channel_free(channel);
        channel = nullptr;
    }
    if (session)
    {
        ssh_disconnect(session);
        ssh_free(session);
        session = nullptr;
    }
}

ssh_connection::operator ssh_session() { return session; }
ssh_connection::operator ssh_channel() { return channel; }

Pty::Pty(Server* server, const char* term, QSize size, bool auto_read, QObject* parent)
    : QObject(parent)
    , data_check(new QTimer{this})
{
    connection = std::move(server->get_ssh_connection());
    
    ssh_channel_request_pty_size(connection, term, size.width(), size.height());
    ssh_channel_request_shell(connection);

    data_check->callOnTimeout(this, &Pty::check_for_data);
    data_check->start(1);

    this->size = size;
}

Pty::~Pty()
{
    delete data_check;
}

void Pty::set_size(QSize size)
{
    if (this->size == size)
        return;
    if (ssh_channel_change_pty_size(connection, size.width(), size.height()) == SSH_ERROR)
        return;
    this->size = size;
}

std::array signal_strings {
    "HUP"sv,
    "INT"sv,
    "QUIT"sv,
    "ILL"sv,
    "TRAP"sv,
    "ABRT"sv,
    "BUS"sv,
    "FPE"sv,
    "KILL"sv,
    "USR1"sv,
    "SEGV"sv,
    "USR2"sv,
    "PIPE"sv,
    "ALRM"sv,
    "TERM"sv,
    "STKFLT"sv,
    "CHLD"sv,
    "CONT"sv,
    "STOP"sv,
    "TSTP"sv,
    "TTIN"sv,
    "TTOU"sv,
    "URG"sv,
    "XCPU"sv,
    "XFSZ"sv,
    "VTALRM"sv,
    "PROF"sv,
    "WINCH"sv,
    "IO"sv,
    "PWR"sv,
    "SYS"sv,
};

void Pty::send_signal(int signal)
{
    ssh_channel_request_send_signal(connection, signal_strings[signal].data());
}

void Pty::check_for_data()
{
    if (!(ssh_channel_is_open(connection) && !ssh_channel_is_eof(connection)))
        return;

    if (auto text = read_stdout(false); text.size() > 0)
        Q_EMIT receved_data(QString::fromStdString(text));
    if (auto text = read_stderr(false); text.size() > 0)
        Q_EMIT receved_data(QString::fromStdString(text));
}

void Pty::send_data(QString data)
{
    auto buffer = data.toUtf8();
    ssh_channel_write(connection, buffer.data(), buffer.size());
}

std::string Pty::read_stdout(bool blocking)
{
    if (!(ssh_channel_is_open(connection) && !ssh_channel_is_eof(connection)))
        return "";
    return read(false, blocking);
}

std::string Pty::read_stderr(bool blocking)
{
    if (!(ssh_channel_is_open(connection) && !ssh_channel_is_eof(connection)))
        return "";
    return read(true, blocking);
}

std::string Pty::read(bool std_err, bool blocking)
{
    char buffer[256] = {0};
    int bytes_receved;
    std::string output;

    if (blocking)
    {
        while (bytes_receved = ssh_channel_read(connection, buffer, sizeof(buffer) - 1, false))
        {
            output += buffer;
            memset(buffer, 0, sizeof(buffer));
        }
    }
    else
    {
        while (bytes_receved = ssh_channel_read_nonblocking(connection, buffer, sizeof(buffer) - 1, false))
        {
            output += buffer;
            memset(buffer, 0, sizeof(buffer));
        }
    }

    return output;
}