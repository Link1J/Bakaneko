// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"
#include <filesystem>
#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>

#if defined(LJH_TARGET_Windows)
#include <ljh/windows/wmi.hpp>
#endif

#include <spdlog/spdlog.h>

#undef interface

extern std::string read_file(std::filesystem::path file_path);
extern std::tuple<int, std::string> exec(const std::string& cmd);

ljh::expected<Bakaneko::Drives, Errors> Info::Drives(const Fields& fields)
{
    decltype(Info::Drives(fields))::value_type drives;

#if defined(LJH_TARGET_Windows)
    using namespace std::string_literals;

    for (auto& drive_info : ljh::windows::wmi::service::root().get_class(L"Win32_DiskDrive"))
    {
        if (!drive_info.get<bool>(L"MediaLoaded") || drive_info.get<uint64_t>(L"Size") == 0)
            continue;

        auto drive = drives.add_drive();

        drive->set_dev_node    (drive_info.get<std::string>(L"DeviceID"     ));
        drive->set_interface   (drive_info.get<std::string>(L"InterfaceType"));
        drive->set_size        (drive_info.get<uint64_t   >(L"Size"         ));
        drive->set_model       (drive_info.get<std::string>(L"Model"        ));
        drive->set_manufacturer(drive_info.get<std::string>(L"Manufacturer" ));

        for (auto& partition_info : drive_info.associators(L"Win32_DiskDriveToDiskPartition"))
        {
            auto partition = drive->add_partition();

            partition->set_dev_node(partition_info.get<std::string>(L"DeviceID"));

            if (auto volumes = partition_info.associators(L"Win32_LogicalDiskToPartition"); volumes.size() > 0)
            {
                auto volume_info = volumes[0];

                partition->set_mountpoint(volume_info.get<std::string>(L"DeviceID"  ));
                partition->set_filesystem(volume_info.get<std::string>(L"FileSystem"));

                auto size =        volume_info.get<uint64_t>(L"Size"     );
                auto used = size - volume_info.get<uint64_t>(L"FreeSpace");

                partition->set_size(size);
                partition->set_used(used);
            }
            else
            {
                auto size = partition_info.get<uint64_t>(L"Size");
                partition->set_size(size);
                partition->set_used(size);
            }
        }
    }
    for (auto& drive_info : ljh::windows::wmi::service::root().get_class(L"Win32_CDROMDrive"))
    {
        if (!drive_info.get<bool>(L"MediaLoaded"))
            continue;
        
        auto drive = drives.add_drive();

        drive->set_dev_node    (drive_info.get<std::string>(L"DeviceID"     ));
        // drive->set_interface   (drive_info.get<std::string>(L"InterfaceType"));
        drive->set_size        (drive_info.get<uint64_t   >(L"Size"         ));
        drive->set_model       (drive_info.get<std::string>(L"Name"         ));
        drive->set_manufacturer(drive_info.get<std::string>(L"Manufacturer" ));

        if (auto volumes = ljh::windows::wmi::service::root().get_class(L"Win32_Volume", L"DriveLetter", drive_info.get(L"Drive")); volumes.size() > 0)
        {
            auto volume_info = volumes[0];
            auto partition = drive->add_partition();

            partition->set_dev_node  (drive_info.get<std::string>(L"VolumeName"));
            partition->set_mountpoint(drive_info.get<std::string>(L"Drive"     ));

            partition->set_filesystem(volume_info.get<std::string>(L"FileSystem"));

            uint64_t size = 0;
            if (volume_info.has(L"Size"))
                size = volume_info.get<uint64_t>(L"Size");
            else
                size = drive_info .get<uint64_t>(L"Size");

            auto used = size - volume_info.get<uint64_t>(L"FreeSpace");

            partition->set_size(size);
            partition->set_used(used);
        }
    }
    std::sort(drives.mutable_drive()->begin(), drives.mutable_drive()->end(),
        [](Bakaneko::Drive& a, Bakaneko::Drive& b) {
            if (a.dev_node().substr(0, 4) == b.dev_node().substr(0, 4))
                return a.dev_node() < b.dev_node();
            return a.dev_node() > b.dev_node();
        }
    );
#elif defined(LJH_TARGET_Linux)
    auto [exit_code, std_out] = exec("lsblk -brn --output NAME,MOUNTPOINT,MODEL,SIZE,FSTYPE,FSSIZE,FSUSED");
    if (exit_code != 0)
        std::tie(exit_code, std_out) = exec("lsblk -brn --output NAME,MOUNTPOINT,MODEL,SIZE,FSTYPE");

    std::set<std::string> drives_seen;
    std::map<std::string, Bakaneko::Drive*> drives_;

    auto drive_strings = ljh::split(std_out, '\n');
    for (auto drive_line : drive_strings)
    {
        if (drive_line.empty())
            continue;

        auto info = ljh::split(drive_line, ' ');

        if (drives_seen.find(info[0]) != drives_seen.end())
            continue;

        auto number = info[0].find_first_of("1234567890");
        bool contains_base_drive = drives_seen.find(info[0].substr(0, number)) != drives_seen.end();

        if (!contains_base_drive)
        {
            auto drive = drives.add_drive();

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
            
            drive->set_dev_node(            info[0] );
            drive->set_model   (            info[2] );
            drive->set_size    (std::stoull(info[3]));

            drives_.emplace(info[0], drive);
        }

        if (number != std::string::npos)
        {
            auto parition = drives_.at(contains_base_drive ? info[0].substr(0, number) : info[0])->add_partition();

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

            parition->set_dev_node  (info[0]);
            parition->set_mountpoint(info[1]);
            parition->set_filesystem(info[4]);

            if (info.size() <= 5)
            {
                auto df = ljh::split(ljh::split(std::get<1>(exec("df -aB1 /dev/" + info[0])), '\n')[1], ' ');
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
                parition->set_size(std::stoull(info[5]));
                parition->set_used(std::stoull(info[6]));
            }
            else
            {
                auto size = std::stoull(info[3]);
                parition->set_size(size);
                parition->set_used(size);
            }
        }

        drives_seen.emplace(info[0]);
    }

    std::sort(drives.mutable_drive()->begin(), drives.mutable_drive()->end(), 
        [](Bakaneko::Drive& a, Bakaneko::Drive& b) {
            auto a2 = a.dev_node().substr(2), b2 = b.dev_node().substr(2);
            if (a2 == b2)
                return a.dev_node() > b.dev_node();
            return a2 < b2;
        }
    );
    for (auto& drive : *drives.mutable_drive())
    {
        std::sort(drive.mutable_partition()->begin(), drive.mutable_partition()->end(),
            [](Bakaneko::Partition& a, Bakaneko::Partition& b) { return a.dev_node() < b.dev_node(); }
        );
    }
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    return std::move(drives);
}