// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "../server.h"

class WindowsComputer : public Server
{
    Q_OBJECT;

public:
    WindowsComputer(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QObject* parent = nullptr);

private:
    std::string get_info     (std::string clazz, std::string key);
    std::string get_env_var  (std::string var                   );
    std::string get_reg_value(std::string path , std::string key);

    std::vector<std::vector<std::string>> powershell(std::string& command, std::vector<std::string>& properties);
    std::vector<std::vector<std::string>> wmic      (std::string class_name, std::string filter, std::string associated, std::vector<std::string> properties);
                std::vector<std::string>  registry  (std::string path, std::vector<std::string> properties);

public:

public Q_SLOTS:
    QString get_kernal_type() override;
    
    void check_for_updates() override;
    void collect_info     () override;
    void collect_drives   () override;

    void shutdown() override;
    void reboot  () override;

Q_SIGNALS:

private:
};
