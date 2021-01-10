// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#include "pty.h"
#include "objects/server.h"

#include <string_view>
#include <array>

using namespace std::literals;

Pty::Pty(Server* server, QObject* parent)
    : QObject(parent)
{
    connection = std::move(server->get_ssh_connection());
    ssh_channel_request_pty_size(connection, "vt100", 80, 24);
    ssh_channel_request_shell(connection);
}

Pty::~Pty() = default;

void Pty::set_size(QSize size)
{
    if (ssh_channel_change_pty_size(connection, size.width(), size.height()) == SSH_ERROR)
        return;
    this->size = size;
    Q_EMIT changed_size();
}

QSize Pty::get_size()
{
    return size;
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