// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"
#include "windows.hpp"

#include <filesystem>
#include <chrono>
#include <mutex>
#include <set>

#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>

#if defined(LJH_TARGET_Windows)
#include <windows.h>
#include <ljh/windows/wmi.hpp>
#elif defined(LJH_TARGET_Linux)
#include <ljh/unix/dbus.hpp>
#include "openrc.hpp"
#endif

#include <spdlog/spdlog.h>

extern std::tuple<int, std::string> exec(const std::string &cmd);

#if defined(LJH_TARGET_Linux)
enum class service_manager
{
    Unknown,
    systemd,
    openrc,
};

service_manager get_service_manager()
{
    static service_manager manager = [] {
        if (rc_service_add)
        {
            spdlog::info("Service Manager: openrc");
            return service_manager::openrc;
        }

        ljh::unix::dbus::connection system_bus(ljh::unix::dbus::bus::SYSTEM);
        auto interface = system_bus.get(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);
        auto dbus_services = interface.call("ListNames").run<std::vector<std::string>>();
        for (auto &service : dbus_services)
        {
            if (service == "org.freedesktop.systemd1")
            {
                spdlog::info("Service Manager: systemd");
                return service_manager::systemd;
            }
        }

        spdlog::warn("Could not determine Service Manager");
        return service_manager::Unknown;
    }();
    return manager;
}
#endif

template <typename... Ts>
std::ostream &operator<<(std::ostream &os, std::tuple<Ts...> const &theTuple)
{
    std::apply(
        [&os](Ts const &...tupleArgs) {
            os << '[';
            std::size_t n{0};
            ((os << tupleArgs << (++n != sizeof...(Ts) ? ", " : "")), ...);
            os << ']';
        },
        theTuple);
    return os;
}

ljh::expected<Bakaneko::ServiceInfo, Errors> Info::Service(const Fields &fields)
{
    Bakaneko::ServiceInfo info;
    info.types.push_back("All");

#if defined(LJH_TARGET_Windows)
    info.server = ("Service Manager");
    info.types.push_back("Process");
    info.types.push_back("Kernel Driver");
    info.types.push_back("File System Driver");
    info.types.push_back("Adapter");
    info.types.push_back("Recongizer Driver");
#elif defined(LJH_TARGET_Linux)
    switch (get_service_manager())
    {
    case service_manager::systemd:
        info.server = ("systemd");
        info.types.push_back("Service");
        info.types.push_back("Socket");
        info.types.push_back("Device");
        info.types.push_back("Mount");
        info.types.push_back("Automount");
        info.types.push_back("Swap");
        info.types.push_back("Target");
        info.types.push_back("Path");
        info.types.push_back("Timer");
        info.types.push_back("Snapshot");
        info.types.push_back("Slice");
        info.types.push_back("Scope");
        break;
    case service_manager::openrc:
        info.server = ("openrc");
        info.types.push_back("Service");
        break;

    default:
        return ljh::unexpected{Errors::NotImplemented};
    }
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    return info;
}

ljh::expected<Bakaneko::Services, Errors> Info::Services(const Fields &fields, Bakaneko::ServicesRequest data)
{
    Bakaneko::Services info;
    std::string type = !data.type.empty() ? data.type : "All";

#if defined(LJH_TARGET_Windows)
    Win32ServiceHandle sc_handle(OpenSCManagerA, nullptr, SERVICES_ACTIVE_DATABASE, GENERIC_READ);

    DWORD types_to_get = 0;
    if (type == "All")
        types_to_get = SERVICE_TYPE_ALL;
    else if (type == "Process")
        types_to_get = SERVICE_WIN32;
    else if (type == "Kernel Driver")
        types_to_get = SERVICE_KERNEL_DRIVER;
    else if (type == "File System Driver")
        types_to_get = SERVICE_FILE_SYSTEM_DRIVER;
    else if (type == "Adapter")
        types_to_get = SERVICE_ADAPTER;
    else if (type == "Recongizer Driver")
        types_to_get = SERVICE_RECOGNIZER_DRIVER;
    else
        return ljh::unexpected{Errors::Failed};

    DWORD bytes_to_get = 0;
    DWORD resume_handle = 0;
    DWORD services_returned = 0;
    EnumServicesStatusExA(sc_handle, SC_ENUM_PROCESS_INFO, types_to_get, SERVICE::STATE::ALL, NULL, 0, &bytes_to_get, &services_returned, &resume_handle, NULL);
    ENUM_SERVICE_STATUS_PROCESSA *service_statuses = (ENUM_SERVICE_STATUS_PROCESSA *)malloc(bytes_to_get);
    EnumServicesStatusExA(sc_handle, SC_ENUM_PROCESS_INFO, types_to_get, SERVICE::STATE::ALL, (LPBYTE)service_statuses, bytes_to_get, &bytes_to_get, &services_returned, &resume_handle, NULL);

    for (int a = 0; a < services_returned; a++)
    {
        auto &service = info.services.emplace_back();

        service.id = (service_statuses[a].lpServiceName);
        service.display_name = (service_statuses[a].lpDisplayName);

        switch (service_statuses[a].ServiceStatusProcess.dwCurrentState)
        {
        case SERVICE_START_PENDING:
            service.state = (Bakaneko::Service::State::Starting);
            break;
        case SERVICE_RUNNING:
            service.state = (Bakaneko::Service::State::Running);
            break;
        case SERVICE_STOP_PENDING:
            service.state = (Bakaneko::Service::State::Stopping);
            break;
        case SERVICE_STOPPED:
            service.state = (Bakaneko::Service::State::Stopped);
            break;
        }

        if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_WIN32) != 0)
            service.type = ("Process");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_KERNEL_DRIVER) != 0)
            service.type = ("Kernel Driver");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_FILE_SYSTEM_DRIVER) != 0)
            service.type = ("File System Driver");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_ADAPTER) != 0)
            service.type = ("Adapter");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_RECOGNIZER_DRIVER) != 0)
            service.type = ("Recongizer Driver");

        Win32ServiceHandle service_handle(OpenServiceA, sc_handle, service_statuses[a].lpServiceName, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);

        QueryServiceConfig2W(service_handle, SERVICE_CONFIG_DESCRIPTION, nullptr, 0, &bytes_to_get);
        SERVICE_DESCRIPTIONW *description = (SERVICE_DESCRIPTIONW *)malloc(bytes_to_get);
        memset(description, 0, bytes_to_get);
        QueryServiceConfig2W(service_handle, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)description, bytes_to_get, &bytes_to_get);

        if (description->lpDescription != nullptr)
            service.description = (ljh::convert_string(description->lpDescription));

        free(description);

        QueryServiceConfigA(service_handle, nullptr, 0, &bytes_to_get);
        QUERY_SERVICE_CONFIGA *config = (QUERY_SERVICE_CONFIGA *)malloc(bytes_to_get);
        memset(config, 0, bytes_to_get);
        QueryServiceConfigA(service_handle, config, bytes_to_get, &bytes_to_get);

        switch (config->dwStartType)
        {
        case SERVICE_AUTO_START:
        case SERVICE_BOOT_START:
        case SERVICE_SYSTEM_START:
            service.enabled = (true);
            break;
        }

        free(config);
    }

    free(service_statuses);
#elif defined(LJH_TARGET_Linux)
    switch (get_service_manager())
    {
    case service_manager::systemd:
        try
        {
            ljh::unix::dbus::connection system_bus(ljh::unix::dbus::bus::SYSTEM);
            auto interface = system_bus.get("org.freedesktop.systemd1", "/org/freedesktop/systemd1", "org.freedesktop.systemd1.Manager");
            auto services = interface.call("ListUnits").run<std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, ljh::unix::dbus::object_path, uint32_t, std::string, ljh::unix::dbus::object_path>>>();
            std::set<std::string> files;
            for (auto &service : services)
            {
                auto service_interface = system_bus.get("org.freedesktop.systemd1", std::get<8>(service).data(), "org.freedesktop.systemd1.Unit");
                auto &service_info = info.services.emplace_back();

                auto path = service_interface.get<std::string>("FragmentPath");
                if (!path.empty())
                    files.emplace(path);

                auto id = std::get<0>(service);
                auto description = std::get<1>(service);

                for (int a = 0; a < id.size() - 1; a++)
                {
                    if (id[a] == '\\' && id[a + 1] == 'x')
                    {
                        auto value = id.substr(a + 2, 2);
                        char letter = std::stoull(value, nullptr, 16);
                        id.insert(id.begin() + a, letter);
                        id.erase(a + 1, 4);
                    }
                }
                for (int a = 0; a < description.size() - 1; a++)
                {
                    if (description[a] == '\\' && description[a + 1] == 'x')
                    {
                        auto value = description.substr(a + 2, 2);
                        char letter = std::stoull(value, nullptr, 16);
                        description.insert(description.begin() + a, letter);
                        description.erase(a + 1, 4);
                    }
                }

                service_info.id = (id);
                service_info.display_name = (id);
                service_info.description = (description);

                auto type = id.substr(id.find_last_of('.') + 1);
                type[0] = std::toupper(type[0]);
                service_info.type = (type);

                auto enabled_state = service_interface.get<std::string>("UnitFileState");
                if (enabled_state == "enabled" || enabled_state == "static")
                    service_info.enabled = (true);

                auto active_state = service_interface.get<std::string>("ActiveState");
                if (active_state == "inactive")
                    service_info.state = (Bakaneko::Service::State::Stopped);
                if (active_state == "failed")
                    service_info.state = (Bakaneko::Service::State::Stopped);
                if (active_state == "active")
                    service_info.state = (Bakaneko::Service::State::Running);
                if (active_state == "activating")
                    service_info.state = (Bakaneko::Service::State::Starting);
                if (active_state == "deactivating")
                    service_info.state = (Bakaneko::Service::State::Stopping);
            }
            auto unloaded = interface.call("ListUnitFiles").run<std::vector<std::tuple<std::string, std::string>>>();
            for (auto &service : unloaded)
            {
                auto id = std::get<0>(service);
                if (files.find(id) != files.end())
                    continue;
                if (id.find('@') != std::string::npos)
                    continue;

                id = id.substr(id.find_last_of('/') + 1);
                auto unit_path = interface.call("LoadUnit").args(id).run<ljh::unix::dbus::object_path>();

                auto service_interface = system_bus.get("org.freedesktop.systemd1", unit_path.data(), "org.freedesktop.systemd1.Unit");
                auto &service_info = info.services.emplace_back();

                auto description = service_interface.get<std::string>("Description");

                for (int a = 0; a < id.size() - 1; a++)
                {
                    if (id[a] == '\\' && id[a + 1] == 'x')
                    {
                        auto value = id.substr(a + 2, 2);
                        char letter = std::stoull(value, nullptr, 16);
                        id.insert(id.begin() + a, letter);
                        id.erase(a + 1, 4);
                    }
                }
                for (int a = 0; a < description.size() - 1; a++)
                {
                    if (description[a] == '\\' && description[a + 1] == 'x')
                    {
                        auto value = description.substr(a + 2, 2);
                        char letter = std::stoull(value, nullptr, 16);
                        description.insert(description.begin() + a, letter);
                        description.erase(a + 1, 4);
                    }
                }

                service_info.id = (id);
                service_info.display_name = (id);
                service_info.description = (description);

                auto type = id.substr(id.find_last_of('.') + 1);
                type[0] = std::toupper(type[0]);
                service_info.type = (type);

                auto enabled_state = service_interface.get<std::string>("UnitFileState");
                if (enabled_state == "enabled" || enabled_state == "static")
                    service_info.enabled = (true);

                auto active_state = service_interface.get<std::string>("ActiveState");
                if (active_state == "inactive")
                    service_info.state = (Bakaneko::Service::State::Stopped);
                if (active_state == "failed")
                    service_info.state = (Bakaneko::Service::State::Stopped);
                if (active_state == "active")
                    service_info.state = (Bakaneko::Service::State::Running);
                if (active_state == "activating")
                    service_info.state = (Bakaneko::Service::State::Starting);
                if (active_state == "deactivating")
                    service_info.state = (Bakaneko::Service::State::Stopping);
            }
        }
        catch (const ljh::unix::dbus::error &e)
        {
            auto error_message = fmt::format("DBus Error: ({}) {}", e.name(), e.message());
            throw std::runtime_error{error_message};
        }
        break;

    case service_manager::openrc:
    {
        auto run_levels = rc_runlevel_list();
        for (auto &service_file_thing : std::filesystem::directory_iterator(RC_INITDIR))
        {
            auto id = service_file_thing.path().filename().string();
            if (!rc_service_exists(id.c_str()))
                continue;

            auto &service = info.services.emplace_back();

            auto desc = rc_service_description(id.c_str(), nullptr);

            service.id = (id);
            service.display_name = (id);
            service.description = (desc);

            free(desc);

            service.type = ("Service");

            for (auto np = run_levels->tqh_first; np != NULL; np = np->entries.tqe_next)
            {
                if (rc_service_in_runlevel(id.c_str(), np->value))
                {
                    service.enabled = (true);
                }
            }

            auto state = rc_service_state(id.c_str());
            if ((state & RC_SERVICE(0xFF)) == RC_SERVICE::STARTED)
                service.state = (Bakaneko::Service::State::Running);
            else if ((state & RC_SERVICE(0xFF)) == RC_SERVICE::STARTING)
                service.state = (Bakaneko::Service::State::Starting);
            else if ((state & RC_SERVICE(0xFF)) == RC_SERVICE::STOPPING)
                service.state = (Bakaneko::Service::State::Stopping);
        }
        rc_stringlist_free(run_levels);
    }
    break;

    default:
        return ljh::unexpected{Errors::NotImplemented};
    }
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    return std::move(info);
}

ljh::expected<void, Errors> Control::Service(const Fields &fields, Bakaneko::Service::Control data)
{
    if (!fields.authentication.has_value())
        return ljh::unexpected{Errors::NeedsPassword};
    if (!Helpers::Authenticate(fields.authentication.value()))
        return ljh::unexpected{Errors::NeedsPassword};

    spdlog::info("Action {} requsested on {}", data.action, data.id);

#if defined(LJH_TARGET_Windows)
    Win32ServiceHandle sc_handle(OpenSCManagerA, nullptr, SERVICES_ACTIVE_DATABASE, GENERIC_READ);
    Win32ServiceHandle service_handle(OpenServiceA, sc_handle, data.id().c_str(), SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG);
    SERVICE_STATUS status;

    switch (data.action())
    {
    case Bakaneko::Service::Control::Action::Stop:
        if (ControlService(service_handle, SERVICE_CONTROL_STOP, &status) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service::Control::Action::Start:
        if (StartServiceA(service_handle, 0, nullptr) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service::Control::Action::Restart:
        if (ControlService(service_handle, SERVICE_CONTROL_STOP, &status) != 0)
            return ljh::unexpected{Errors::Failed};
        if (StartServiceA(service_handle, 0, nullptr) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service::Control::Action::Enable:
        if (ChangeServiceConfigA(service_handle, SERVICE_NO_CHANGE, SERVICE_AUTO_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service::Control::Action::Disable:
        if (ChangeServiceConfigA(service_handle, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    }
#elif defined(LJH_TARGET_Linux)
    switch (get_service_manager())
    {
    case service_manager::systemd:
        try
        {
            bool _1;
            std::vector<std::tuple<std::string, std::string, std::string>> _2;

            ljh::unix::dbus::connection system_bus(ljh::unix::dbus::bus::SYSTEM);
            auto interface = system_bus.get("org.freedesktop.systemd1", "/org/freedesktop/systemd1", "org.freedesktop.systemd1.Manager");
            auto unit_path = interface.call("LoadUnit").args(data.id).run<ljh::unix::dbus::object_path>();
            auto service_interface = system_bus.get("org.freedesktop.systemd1", unit_path.data(), "org.freedesktop.systemd1.Unit");
            switch (data.action)
            {
            case Bakaneko::Service::Control::Action::Stop:
                service_interface.call("Stop").args("replace").run<ljh::unix::dbus::object_path>();
                break;
            case Bakaneko::Service::Control::Action::Start:
                service_interface.call("Start").args("replace").run<ljh::unix::dbus::object_path>();
                break;
            case Bakaneko::Service::Control::Action::Restart:
                service_interface.call("Restart").args("replace").run<ljh::unix::dbus::object_path>();
                break;
            case Bakaneko::Service::Control::Action::Enable:
            {
                auto path = service_interface.get<std::string>("FragmentPath");
                if (path.empty())
                    return ljh::unexpected{Errors::Failed};
                interface.call("EnableUnitFiles").args(std::vector{data.id}, false, false).run(_1, _2);
                for (auto &s : _2)
                {
                    spdlog::debug("{}, {}, {}", std::get<0>(s), std::get<1>(s), std::get<2>(s));
                }
            }
            break;
            case Bakaneko::Service::Control::Action::Disable:
            {
                auto path = service_interface.get<std::string>("FragmentPath");
                if (path.empty())
                    return ljh::unexpected{Errors::Failed};
                interface.call("DisableUnitFiles").args(std::vector{data.id}, false).run(_2);
            }
            break;
            default:
                return ljh::unexpected{Errors::NotImplemented};
            }
        }
        catch (const ljh::unix::dbus::error &e)
        {
            auto error_message = fmt::format("DBus Error: ({}) {}", e.name(), e.message());
            throw std::runtime_error{error_message};
        }
        break;

    case service_manager::openrc:
    {
        auto path = [data] {
            auto temp = rc_service_resolve(data.id.c_str());
            std::string out = temp;
            free(temp);
            return out;
        }();

        switch (data.action)
        {
        case Bakaneko::Service::Control::Action::Stop:
        {
            auto [code, std_out] = exec(path + " stop");
            if (code != 0)
                return ljh::unexpected{Errors::Failed};
        }
        break;
        case Bakaneko::Service::Control::Action::Start:
        {
            auto [code, std_out] = exec(path + " start");
            if (code != 0)
                return ljh::unexpected{Errors::Failed};
        }
        break;
        case Bakaneko::Service::Control::Action::Restart:
        {
            auto [code, std_out] = exec(path + " restart");
            if (code != 0)
                return ljh::unexpected{Errors::Failed};
        }
        break;
        case Bakaneko::Service::Control::Action::Enable:
        {
            if (!rc_service_add("default", data.id.c_str()))
                return ljh::unexpected{Errors::Failed};
        }
        break;
        case Bakaneko::Service::Control::Action::Disable:
        {
            auto run_levels = rc_runlevel_list();
            for (auto np = run_levels->tqh_first; np != NULL; np = np->entries.tqe_next)
            {
                if (rc_service_in_runlevel(data.id.c_str(), np->value))
                {
                    rc_service_delete(np->value, data.id.c_str());
                }
            }
            rc_stringlist_free(run_levels);
        }
        break;
        default:
            return ljh::unexpected{Errors::NotImplemented};
        }
    }
    break;

    default:
        return ljh::unexpected{Errors::NotImplemented};
    }
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    return ljh::expected<void, Errors>{};
}