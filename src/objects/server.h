// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QTimer>

#include <memory>

#include <libssh/libssh.h>

#include <term/pty.h>

class Server : public QObject
{
    friend class ServerManager;
    Q_OBJECT

public:
    Server(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QObject* parent = nullptr);
    ~Server();

    enum class State
    {
        Online,
        Offline,
    };
    Q_ENUM(State)

private:
    Q_PROPERTY(QString hostname    READ get_hostname    NOTIFY changed_hostname   )
    Q_PROPERTY(State   state       READ get_state       NOTIFY changed_state      )
    Q_PROPERTY(QString ip          READ get_ip          NOTIFY changed_ip         )
    Q_PROPERTY(QString mac         READ get_mac         NOTIFY changed_mac        )
    Q_PROPERTY(QString system_icon READ get_system_icon NOTIFY changed_system_icon)
    Q_PROPERTY(QString system_type READ get_system_type NOTIFY changed_system_type)
    Q_PROPERTY(QString os          READ get_os          NOTIFY changed_os         )
    Q_PROPERTY(QString kernel      READ get_kernel      NOTIFY changed_kernel     )
    Q_PROPERTY(QString arch        READ get_arch        NOTIFY changed_arch       )
    Q_PROPERTY(QString vm_platform READ get_vm_platform NOTIFY changed_vm_platform)

    std::tuple<int, std::string, std::string> run_command(std::string command);

public:
    ssh_connection get_ssh_connection();

public Q_SLOTS:
    Q_INVOKABLE QString get_hostname   ();
    Q_INVOKABLE State   get_state      ();
    Q_INVOKABLE QString get_os         ();
    Q_INVOKABLE QString get_ip         ();
    Q_INVOKABLE QString get_mac        ();
    Q_INVOKABLE QString get_system_icon();
    Q_INVOKABLE QString get_system_type();
    Q_INVOKABLE QString get_kernel     ();
    Q_INVOKABLE QString get_arch       ();
    Q_INVOKABLE QString get_vm_platform();

    void update_info      ();
    //void check_for_updates();

    Q_INVOKABLE void wake_up ();
    Q_INVOKABLE void shutdown();
    Q_INVOKABLE void reboot  ();

Q_SIGNALS:
    void changed_hostname   ();
    void changed_state      ();
    void changed_ip         ();
    void changed_mac        ();
    void changed_system_icon();
    void changed_system_type();
    void changed_os         ();
    void changed_kernel     ();
    void changed_arch       ();
    void changed_vm_platform();

private:
    QString hostname;
    QString ip_address;
    QString mac_address;
    QString username;
    QString password;
    State   state = State::Offline;
    QString system_icon;
    QString system_type;
    QString operating_system;
    QString kernel;
    QString architecture;
    QString vm_platform = "";
    QString pretty_hostname = "";
};

using ServerPointer = Server*;
Q_DECLARE_METATYPE(ServerPointer);
