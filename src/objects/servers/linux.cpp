// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "linux.h"
#include <managers/servermanager.h>

static std::vector<std::string> split(const std::string& s, char seperator)
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

LinuxComputer::LinuxComputer(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QObject* parent)
    : Server(hostname, mac_address, ip_address, username, password, parent)
{
    if (this->hostname.isEmpty())
    {
        auto [exit_code, std_out, std_err] = run_command("cat /etc/hostname");
        this->hostname = QString::fromStdString(std_out.substr(0, std_out.find('\n')));
    }
    if (this->mac_address.isEmpty())
    {
        auto [exit_code, std_out, std_err] = run_command("cat /sys/class/net/*/address");
        this->mac_address = QString::fromStdString(std_out.substr(0, std_out.find('\n')));
    }
}

/*
    For shutdown to work, sudo must be installed and the line below added to /etc/sudoers
    user hostname =NOPASSWD: /usr/bin/systemctl poweroff,/usr/bin/systemctl reboot
*/
void LinuxComputer::shutdown()
{
    run_command("sudo systemctl poweroff");
}

/*
    For shutdown to work, sudo must be installed and the line below added to /etc/sudoers
    user hostname =NOPASSWD: /usr/bin/systemctl poweroff,/usr/bin/systemctl reboot
*/
void LinuxComputer::reboot()
{
    run_command("sudo systemctl reboot");
}

void LinuxComputer::check_for_updates()
{
    UpdateList data;

    auto run = [this, &data](std::string command, bool(*decode)(Update*, std::string)) {
        if (data.size() == 0)
        {
            auto [exit_code, std_out, std_err] = run_command(command);
            if (exit_code == 0)
            {
                auto packages = split(std_out, '\n');
                for (auto& package : packages)
                {
                    if (package.empty())
                        continue;

                    Update* update = new Update;
                    decode(update, package);
                    data.append(update);
                }
            }
        }
    };

    auto arch_decode = [](Update* update, std::string package) {
        size_t space = package.find(' '), pre_space = 0;
        update->m_name = QString::fromStdString(package.substr(pre_space, space - pre_space));

        pre_space = space;
        space = package.find(' ', space + 1);
        update->m_old_version = QString::fromStdString(package.substr(pre_space, space - pre_space));

        pre_space = space + 4;
        update->m_new_version = QString::fromStdString(package.substr(pre_space));

        return true;
    };
    run("yay -Qu"   , arch_decode);
    run("pacman -Qu", arch_decode);

    run("apt list --upgradable", [](Update* update, std::string package) {
        auto info = split(package, ' ');
        if (info.size() != 6)
            return false;

        update->m_name        = QString::fromStdString(info[0].substr(0, info[0].find('/')));
        update->m_old_version = QString::fromStdString(info[5].substr(0, info[5].size()-1 ));
        update->m_new_version = QString::fromStdString(info[1]                             );

        return true;
    });

    if (updates.size() != data.size())
    {
        for (int a = 0; a < updates.size(); a++)
            delete updates[a];
        updates = data;
        Q_EMIT new_updates(updates);
        Q_EMIT changed_updates();
    }
}

QString LinuxComputer::get_kernal_type()
{
    return "Linux";
}

void LinuxComputer::collect_info()
{
    auto [exit_code, std_out, std_err] = run_command("hostnamectl status");

    if (exit_code != 0)
    {
        system_icon = "";
        Q_EMIT changed_system_icon();
        return;
    }

    auto get_value = [](std::string& std_out, std::string key) -> std::string {
        auto text_pos = std_out.find(key) + key.length() + 2;
        if (text_pos == std::string::npos + key.length() + 2)
            return "";
        auto newline_pos = std_out.find('\n', text_pos);
        return std_out.substr(text_pos, newline_pos - text_pos);
    };

    operating_system = QString::fromStdString(get_value(std_out, "Operating System"));
    kernel           = QString::fromStdString(get_value(std_out, "Kernel"          ));
    system_icon      = QString::fromStdString(get_value(std_out, "Icon name"       ));
    system_type      = QString::fromStdString(get_value(std_out, "Chassis"         ));
    architecture     = QString::fromStdString(get_value(std_out, "Architecture"    ));
    pretty_hostname  = QString::fromStdString(get_value(std_out, "Pretty hostname" ));
    vm_platform      = QString::fromStdString(get_value(std_out, "Virtualization"  ));

    Q_EMIT changed_system_icon();
    Q_EMIT changed_system_type();
    Q_EMIT changed_os         ();
    Q_EMIT changed_kernel     ();
    Q_EMIT changed_arch       ();
    Q_EMIT changed_hostname   ();
    Q_EMIT changed_vm_platform();
    ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
}