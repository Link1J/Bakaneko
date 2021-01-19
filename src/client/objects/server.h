// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QTimer>

#include <memory>
#include <mutex>
#include <array>
#include <vector>
#include <string>

#include <libssh/libssh.h>

#include <term/pty.h>
#include "update.h"
#include "drives.h"

class Server : public QObject
{
    friend class ServerManager;
    Q_OBJECT;

protected:
    Server(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QObject* parent = nullptr);

public:
    static Server* create(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QString kernal_type);
    virtual ~Server();

    enum class State
    {
        Online,
        Offline,
        Unknown,
    };
    Q_ENUM(State);

    Q_PROPERTY(QString        hostname    READ get_hostname    NOTIFY changed_hostname   );
    Q_PROPERTY(State          state       READ get_state       NOTIFY changed_state      );
    Q_PROPERTY(QString        ip          READ get_ip          NOTIFY changed_ip         );
    Q_PROPERTY(QString        mac         READ get_mac         NOTIFY changed_mac        );
    Q_PROPERTY(QString        system_icon READ get_system_icon NOTIFY changed_system_icon);
    Q_PROPERTY(QString        system_type READ get_system_type NOTIFY changed_system_type);
    Q_PROPERTY(QString        os          READ get_os          NOTIFY changed_os         );
    Q_PROPERTY(QString        kernel      READ get_kernel      NOTIFY changed_kernel     );
    Q_PROPERTY(QString        arch        READ get_arch        NOTIFY changed_arch       );
    Q_PROPERTY(QString        vm_platform READ get_vm_platform NOTIFY changed_vm_platform);
    Q_PROPERTY(QList<Update*> updates     READ get_updates     NOTIFY changed_updates    );
    Q_PROPERTY(QList<Drive*>  drives      READ get_drives      NOTIFY changed_drives     );

protected:
    std::tuple<int, std::string, std::string> run_command(std::string command);

public:
    ssh_connection get_ssh_connection();

public Q_SLOTS:
    Q_INVOKABLE QString       get_hostname   ();
    Q_INVOKABLE State         get_state      ();
    Q_INVOKABLE QString       get_os         ();
    Q_INVOKABLE QString       get_ip         ();
    Q_INVOKABLE QString       get_mac        ();
    Q_INVOKABLE QString       get_system_icon();
    Q_INVOKABLE QString       get_system_type();
    Q_INVOKABLE QString       get_kernel     ();
    Q_INVOKABLE QString       get_arch       ();
    Q_INVOKABLE QString       get_vm_platform();
    Q_INVOKABLE UpdateList    get_updates    ();
    Q_INVOKABLE QList<Drive*> get_drives     ();

    virtual QString get_kernal_type() = 0;

            void  update_info      ();
    virtual void  check_for_updates() = 0;
    virtual void  collect_info     () = 0;
            State ping_computer    ();
    virtual void  collect_drives   () = 0;

            void wake_up ();
    virtual void shutdown() = 0;
    virtual void reboot  () = 0;

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
    void changed_updates    ();
    void changed_drives     ();

    void new_updates(QList<Update*>);

    void this_online (Server*);
    void this_offline(Server*);

protected:
    QString       hostname;
    QString       ip_address;
    QString       mac_address;
    QString       username;
    QString       password;
    State         state = State::Unknown;
    QString       system_icon;
    QString       system_type;
    QString       operating_system;
    QString       kernel;
    QString       architecture;
    QString       vm_platform = "";
    QString       pretty_hostname = "";
    UpdateList    updates;
    QList<Drive*> drives;

    std::mutex update_lock;
};

using ServerPointer = Server*;
Q_DECLARE_METATYPE(ServerPointer);

inline std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

template<size_t C>
std::vector<std::string> split(const std::string& s, const char (&seperator)[C])
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos, C - 1)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = pos += C - 1;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

inline std::string bytes_to_string(uint64_t size)
{
    using namespace std::string_literals;
    int count;
    const std::array size_end = { " B"s, "KB"s, "MB"s, "GB"s, "TB"s };
    for (count = 0; size > 1024 && count < size_end.size(); count++, size /= 1024);
    return std::to_string(size) + size_end[count];
}