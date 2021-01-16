// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "windows.h"
#include <managers/servermanager.h>

std::string chassis_type_as_string(int a)
{
    switch(a)
    {
    case 0x01: return "Other";
    case 0x02: return "Unknown";
    case 0x03: return "Desktop";
    case 0x04: return "Low Profile Desktop";
    case 0x05: return "Pizza Box";
    case 0x06: return "Mini Tower";
    case 0x07: return "Tower";
    case 0x08: return "Portable";
    case 0x09: return "Laptop";
    case 0x0A: return "Notebook";
    case 0x0B: return "Hand Held";
    case 0x0C: return "Docking Station";
    case 0x0D: return "All in One";
    case 0x0E: return "Sub Notebook";
    case 0x0F: return "Space-saving";
    case 0x10: return "Lunch Box";
    case 0x11: return "Main Server Chassis";
    case 0x12: return "Expansion Chassis";
    case 0x13: return "SubChassis";
    case 0x14: return "Bus Expansion Chassis";
    case 0x15: return "Peripheral Chassis";
    case 0x16: return "RAID Chassis";
    case 0x17: return "Rack Mount Chassis";
    case 0x18: return "Sealed-case PC";
    case 0x19: return "Multi-system chassis";
    case 0x1A: return "Compact PCI";
    case 0x1B: return "Advanced TCA";
    case 0x1C: return "Blade";
    case 0x1D: return "Blade Enclosure";
    case 0x1E: return "Tablet";
    case 0x1F: return "Convertible";
    case 0x20: return "Detachable";
    case 0x21: return "IoT Gateway";
    case 0x22: return "Embedded PC";
    case 0x23: return "Mini PC";
    case 0x24: return "Stick PC";
    }
    return "";
}

std::string chassis_type_as_system_type(int a)
{
    auto data = chassis_type_as_string(a);
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return std::tolower(c); });
    return data;
}

std::string chassis_type_as_system_icon(int a)
{
    switch(a)
    {
    case 0x01: // Other
    case 0x03: // Desktop
    case 0x04: // Low Profile Desktop
    case 0x05: // Pizza Box
    case 0x06: // Mini Tower
    case 0x07: // Tower
    case 0x0D: // All in One
    case 0x10: // Lunch Box
    case 0x22: // Embedded PC
    case 0x23: // Mini PC
    case 0x24: // Stick PC
        return "computer";

    case 0x08: // Portable
    case 0x09: // Laptop
    case 0x0A: // Notebook
    case 0x0C: // Docking Station
    case 0x0E: // Sub Notebook
    case 0x1F: // Convertible
    case 0x20: // Detachable
        return "computer-laptop";

    case 0x11: // Main Server Chassis
    case 0x12: // Expansion Chassis
    case 0x13: // SubChassis
    case 0x14: // Bus Expansion Chassis
    case 0x15: // Peripheral Chassis
    case 0x16: // RAID Chassis
    case 0x17: // Rack Mount Chassis
    case 0x1C: // Blade
    case 0x1D: // Blade Enclosure
        return "network-server";

    case 0x0B: // Hand Held
        return "phone";

    case 0x1E: // Tablet
        return "tablet";

    case 0x02: // Unknown
    case 0x0F: // Space-saving
    case 0x18: // Sealed-case PC
    case 0x19: // Multi-system chassis
    case 0x1A: // Compact PCI
    case 0x1B: // Advanced TCA
    case 0x21: // IoT Gateway
        return "unknown";
    }

    return "";
}

WindowsComputer::WindowsComputer(QString hostname, QString mac_address, QString ip_address, QString username, QString password, QObject* parent)
    : Server(hostname, mac_address, ip_address, username, password, parent)
{
    if (this->hostname.isEmpty() || this->mac_address.isEmpty())
    {
        auto [exit_code, std_out, std_err] = run_command("ipconfig /all");

        auto get_value = [](std::string& std_out, std::string key) -> std::string {
            auto text_pos = std_out.find(key) + key.length();
            if (text_pos == std::string::npos + key.length())
                return "";
            text_pos = std_out.find(':', text_pos) + 2;
            auto newline_pos = std_out.find('\n', text_pos);
            return std_out.substr(text_pos, newline_pos - text_pos);
        };

        this->hostname = QString::fromStdString(get_value(std_out, "Host Name"));

        auto temp = get_value(std_out, "Physical Address");
        std::replace(temp.begin(), temp.end(), '-', ':');
        this->mac_address = QString::fromStdString(temp);
    }
}

void WindowsComputer::shutdown()
{
    run_command("shutdown /s /t 0");
}

void WindowsComputer::reboot()
{
    run_command("shutdown /r /t 0");
}

void WindowsComputer::check_for_updates()
{
}

QString WindowsComputer::get_kernal_type()
{
    return "Windows";
}

void WindowsComputer::collect_info()
{
    std::thread os_thread{[this]() {
        auto os_name = get_info("OS", "Caption");
        if (os_name.find("Microsoft ") == 0)
            os_name = os_name.substr(10);

        if (auto line = os_name.find("Windows 10"); line != std::string::npos)
        {
            line += 10;
            auto version = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "DisplayVersion");
            if (version == "")
                version = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ReleaseId");
            if (version != "")
                os_name = os_name.substr(0, line + 1) + version + os_name.substr(line);
        }
        else
        {
            line += os_name.find_first_of(' ', line + 11);
            auto version = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CSDVersion");
            if (version != "")
                os_name = os_name.substr(0, line + 1) + version + os_name.substr(line);
        }

        operating_system = QString::fromStdString(os_name);
    }};

    std::thread kernel_thread{[this]() {
        auto kernel_version = "NT " + get_info("OS", "Version");
        auto UBR = get_reg_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "UBR");
        if (UBR != "") kernel_version += "." + std::to_string(std::stoi(UBR, nullptr, 0));

        kernel           = QString::fromStdString(kernel_version);
    }};

    std::thread architecture_thread{[this]() {
        architecture     = QString::fromStdString(get_env_var("PROCESSOR_ARCHITECTURE"));
    }};

    std::thread system_thread{[this]() {
        int chassis_type = std::stoi(get_info("SystemEnclosure", "ChassisTypes").substr(1));
        system_icon      = QString::fromStdString(chassis_type_as_system_icon(chassis_type));
        system_type      = QString::fromStdString(chassis_type_as_system_type(chassis_type));
    }};

    os_thread          .join();
    kernel_thread      .join();
    architecture_thread.join();
    system_thread      .join();

    Q_EMIT changed_system_icon();
    Q_EMIT changed_system_type();
    Q_EMIT changed_os         ();
    Q_EMIT changed_kernel     ();
    Q_EMIT changed_arch       ();

    if (ServerManager::Instance().GetModel())
        ServerManager::Instance().GetModel()->update(ServerManager::Instance().GetIndex(this));
}

std::string WindowsComputer::get_info(std::string clazz, std::string key)
{
    auto [exit_code, std_out, std_err] = run_command("wmic " + clazz + " get " + key);
    if (exit_code != 0) return "";

    auto newline_pos1 = std_out.find('\n') + 1;
    auto newline_pos2 = std_out.find_last_not_of(' ', std_out.find('\r', newline_pos1) - 1) + 1;
    return std_out.substr(newline_pos1, newline_pos2 - newline_pos1);
};

std::string WindowsComputer::get_env_var(std::string var) {
    auto [exit_code, std_out, std_err] = run_command("echo %" + var + "%");

    if (std_out.find("%"+var+"%") != std::string::npos)
        std::tie(exit_code, std_out, std_err) = run_command("echo $env:" + var);

    if (exit_code != 0) return "";

    return std_out.substr(0, std_out.find('\n'));
};

std::string WindowsComputer::get_reg_value(std::string path, std::string key) {
    auto command = "reg query \\\"" + path + "\\\" /v " + key;
    auto [exit_code, std_out, std_err] = run_command(command);

    if (exit_code != 0) return "";

    auto start = std_out.find("REG_");
    start = std_out.find(' ', start);
    start = std_out.find_first_not_of(' ', start);
    auto length = std_out.find("\r", start) - start;
    return std_out.substr(start, length);
};
