// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "drives.hpp"
#include <ljh/system_info.hpp>

#if defined(LJH_TARGET_Windows)
#include <ljh/windows/wmi.hpp>
#endif

#undef interface
#include "server.pb.h"

Http::Response Info::Drives(const Http::Request& request)
{
    Bakaneko::Drives drives;

#if defined(LJH_TARGET_Windows)
    using namespace ljh::windows::com_bstr_literals;
    using namespace std::string_literals;

    static ljh::windows::wmi::service service{L"ROOT\\CIMV2"_bstr};

    for (auto& drive_info : service.get_class(L"Win32_DiskDrive"))
    {
        if (!drive_info.has(L"Size"))
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
#endif

    return Http::output_message(drives, request);
}