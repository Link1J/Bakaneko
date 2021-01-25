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

#include <models/updatemodel.h>
#include <models/drivesmodel.h>

#undef interface
#include "server.pb.h"
#include "drives.pb.h"
#include "updates.pb.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace asio  = boost::asio ;
namespace beast = boost::beast;

class Server : public QObject
{
    friend class ServerManager;
    Q_OBJECT;

protected:

public:
    Server(std::string hostname, std::string mac_address, std::string ip_address, QObject* parent = nullptr);
    virtual ~Server();

    enum class State
    {
        Unknown,
        Online,
        Offline,
    };
    Q_ENUM(State);

    Q_PROPERTY(QString      hostname READ get_hostname CONSTANT);
    Q_PROPERTY(QString      ip       READ get_ip       CONSTANT);
    Q_PROPERTY(QString      mac      READ get_mac      CONSTANT);
    Q_PROPERTY(State        state    READ get_state    NOTIFY changed_state   );
    Q_PROPERTY(QString      icon     READ get_icon     NOTIFY changed_icon    );
    Q_PROPERTY(QString      os       READ get_os       NOTIFY changed_os      );
    Q_PROPERTY(QString      kernel   READ get_kernel   NOTIFY changed_kernel  );
    Q_PROPERTY(QString      arch     READ get_arch     NOTIFY changed_arch    );
    Q_PROPERTY(UpdateModel* updates  READ get_updates  CONSTANT);
    Q_PROPERTY(DrivesModel* drives   READ get_drives   CONSTANT);

protected:
    std::tuple<int, std::string, std::string> run_command(std::string command);
    
    template<typename T>
    void network_get(std::string path, void(Server::*signal)(T));
    void network_post(std::string path);

public:
    ssh_connection get_ssh_connection();

    asio::ip::tcp::socket& connection();

    std::atomic<int> steps_done = 3;
    static constexpr auto max_steps = 3;

public Q_SLOTS:
    State        get_state   ();

    QString      get_hostname();
    QString      get_ip      ();
    QString      get_mac     ();
    
    QString      get_os      ();
    QString      get_icon    ();
    QString      get_kernel  ();
    QString      get_arch    ();

    UpdateModel* get_updates ();
    DrivesModel* get_drives  ();

    void  update_info    ();
    State ping_computer  ();

    void handle_info   (Bakaneko::System );
    void handle_drives (Bakaneko::Drives );
    void handle_updates(Bakaneko::Updates);

    void wake_up ();
    void shutdown();
    void reboot  ();

    void open_term(QObject* page, QObject* login, QString username, QString password);

Q_SIGNALS:
    void changed_state   ();

    void changed_hostname();
    void changed_ip      ();
    void changed_mac     ();

    void changed_os      ();
    void changed_icon    ();
    void changed_kernel  ();
    void changed_arch    ();

    void changed_updates ();
    void changed_drives  ();
    
    void got_info   (Bakaneko::System );
    void got_drives (Bakaneko::Drives );
    void got_updates(Bakaneko::Updates);

    void this_online (Server*);
    void this_offline(Server*);

    void open_term      (QVariant pty);
    void connecting_fail(QVariant msg);

protected:
    asio::ip::tcp::socket socket;

    State state;

    std::string hostname;
    std::string ip_address;
    std::string mac_address;

    Bakaneko::System system_info;

    UpdateModel updates;
    DrivesModel drives ;

    std::mutex update_lock;
    std::mutex socket_lock;
};

using ServerPointer = Server*;
Q_DECLARE_METATYPE(ServerPointer);

Q_DECLARE_METATYPE(Bakaneko::System );
Q_DECLARE_METATYPE(Bakaneko::Drives );
Q_DECLARE_METATYPE(Bakaneko::Updates);

template<size_t factor = 1024>
inline std::string bytes_to_string(uint64_t size)
{
    using namespace std::string_literals;
    int count;
    const std::array size_end = { " B"s, "KB"s, "MB"s, "GB"s, "TB"s };
    for (count = 0; size >= factor && count < size_end.size(); count++, size /= factor);
    return std::to_string(size) + size_end[count];
}