// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "linux.h"
#include <managers/servermanager.h>

#include <set>
#include <string>
#include <array>
#include <algorithm>

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
                    if (decode(update, package))
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

    if (ServerManager::Instance().GetModel())
        ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
}

void LinuxComputer::collect_drives()
{
    auto [exit_code, std_out, std_err] = run_command("lsblk -brn --output NAME,MOUNTPOINT,MODEL,SIZE,FSTYPE,FSSIZE,FSUSE%");
    if (exit_code != 0)
        std::tie(exit_code, std_out, std_err) = run_command("lsblk -brn --output NAME,MOUNTPOINT,MODEL,SIZE,FSTYPE");

    std::set<std::string> drives_seen;

    for (auto& drive : drives)
    {
        delete drive;
        drive = nullptr;
    }
    drives.clear();

    auto drive_strings = split(std_out, '\n');
    for (auto drive_line : drive_strings)
    {
        if (drive_line.empty())
            continue;

        auto info = split(drive_line, ' ');

        if (drives_seen.find(info[0]) != drives_seen.end())
            continue;

        auto number = info[0].find_first_of("1234567890");
        bool contains_base_drive = drives_seen.find(info[0].substr(0, number)) != drives_seen.end();

        if (!contains_base_drive)
        {
            auto drive = new Drive;

            for (int a = 0; a < info[2].size(); a++)
            {
                if (info[2][a] == '\\' && info[2][a + 1] == 'x')
                {
                    size_t idx = 0;
                    auto number = info[2].substr(a + 2, 2);
                    char charar = std::stoi(number, &idx, 16);
                    info[2][a] = charar;

                    auto top = info[2].substr(0, a + 1);
                    auto bot = info[2].substr(a + 2 + idx);
                    info[2] = top + bot;
                }
            }
            
            drive->m_dev_node = QString::fromStdString(info[0]);
            drive->m_model    = QString::fromStdString(info[2]);

            drive->m_size = QString::fromStdString(bytes_to_string(std::stoull(info[3])));

            drives.push_front(drive);
        }

        if (number != std::string::npos)
        {
            auto parition = new Parition;

            for (int a = 0; a < info[1].size(); a++)
            {
                if (info[1][a] == '\\' && info[1][a + 1] == 'x')
                {
                    size_t idx = 0;
                    auto number = info[1].substr(a + 2, 2);
                    char charar = std::stoi(number, &idx, 16);
                    info[1][a] = charar;

                    auto top = info[1].substr(0, a + 1);
                    auto bot = info[1].substr(a + 2 + idx);
                    info[1] = top + bot;
                }
            }

            parition->m_dev_node   = QString::fromStdString(info[0]);
            parition->m_mountpoint = QString::fromStdString(info[1]);
            parition->m_filesystem = QString::fromStdString(info[4]);

            if (info.size() <= 5)
            {
                auto df = split(split(std::get<1>(run_command("df -aB1 /dev/" + info[0])), '\n')[1], ' ');
                for (auto s = df.begin(); s != df.end(); s++)
                {
                    if (s->empty())
                    {
                        s = --df.erase(s);
                    }
                }
                if (df[0] == "/dev/" + info[0])
                {
                    info.push_back(df[1]);
                    info.push_back(df[4]);
                }
                else
                {
                    info.push_back("");
                    info.push_back("");
                }
            }

            if (!info[5].empty())
            {
                parition->m_size = QString::fromStdString(bytes_to_string(std::stoull(info[5])));
                parition->m_used = QString::fromStdString(info[6]);
            }
            else
            {
                parition->m_size = QString::fromStdString(bytes_to_string(std::stoull(info[3])));
                parition->m_used = "100%";
            }

            auto from = QString::fromStdString(
                contains_base_drive
                ? info[0].substr(0, number)
                : info[0]
            );

            for (auto& drive : drives)
            {
                if (drive->m_dev_node == from)
                {
                    drive->m_paritions.append(parition);
                    break;
                }
            }
        }

        drives_seen.emplace(info[0]);
    }

    std::sort(drives.begin(), drives.end(), [](Drive* a, Drive* b) {
        auto a2 = a->m_dev_node.left(2), b2 = b->m_dev_node.left(2);
        if (a2 == b2)
            return a->m_dev_node < b->m_dev_node;
        return a2 > b2;
    });
    for (auto& drive : drives)
    {
        std::sort(drive->m_paritions.begin(), drive->m_paritions.end(), [](Parition* a, Parition* b) { return a->m_dev_node < b->m_dev_node; });
    }
    

    Q_EMIT changed_drives();
}