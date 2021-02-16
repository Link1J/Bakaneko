// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"
#include "windows.hpp"

#include <filesystem>
#include <chrono>
#include <mutex>

#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>

#if defined(LJH_TARGET_Windows)
#include <windows.h>
#include <ljh/windows/wmi.hpp>
#elif defined(LJH_TARGET_Linux)
#include <ljh/unix/dbus.hpp>
#endif

#include <spdlog/spdlog.h>

enum class service_manager 
{
    Unknown, systemd, 
};

service_manager get_service_manager()
{
    static service_manager manager = service_manager::Unknown;
    static bool ran = false;
    static std::mutex mutex;

    std::lock_guard _(mutex);
    if (!ran)
    {
        ljh::unix::dbus::connection system_bus(ljh::unix::dbus::bus::SYSTEM);
        auto interface = system_bus.get(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);
        auto dbus_services = interface.call("ListNames").run<std::vector<std::string>>();
        for (auto& service : dbus_services)
        {
            if (service == "org.freedesktop.systemd1")
            {
                manager = service_manager::systemd;
                break;
            }
        }
    }
    return manager;
}

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, std::tuple<Ts...> const& theTuple)
{
    std::apply
    (
        [&os](Ts const&... tupleArgs)
        {
            os << '[';
            std::size_t n{0};
            ((os << tupleArgs << (++n != sizeof...(Ts) ? ", " : "")), ...);
            os << ']';
        }, theTuple
    );
    return os;
}

ljh::expected<Bakaneko::ServiceInfo, Errors> Info::Service(const Fields& fields)
{
    Bakaneko::ServiceInfo info;
    *info.add_types() = "All";

#if defined(LJH_TARGET_Windows)
    info.set_server("Service Manager");
    *info.add_types() = "Process";
    *info.add_types() = "Kernel Driver";
    *info.add_types() = "File System Driver";
    *info.add_types() = "Adapter";
    *info.add_types() = "Recongizer Driver";
#elif defined(LJH_TARGET_Linux)
    switch (get_service_manager())
    {
    case service_manager::systemd:
        info.set_server("systemd");
        *info.add_types() = "Service";
        *info.add_types() = "Socket";
        *info.add_types() = "Device";
        *info.add_types() = "Mount";
        *info.add_types() = "Automount";
        *info.add_types() = "Swap";
        *info.add_types() = "Target";
        *info.add_types() = "Path";
        *info.add_types() = "Timer";
        *info.add_types() = "Snapshot";
        *info.add_types() = "Slice";
        *info.add_types() = "Scope";
        break;
    
    default:
        return ljh::unexpected{Errors::NotImplemented};
    }
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    return info;
}

ljh::expected<Bakaneko::Services, Errors> Info::Services(const Fields& fields, Bakaneko::ServicesRequest data)
{
    Bakaneko::Services info;
    std::string type = !data.type().empty() ? data.type() : "All";

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
    EnumServicesStatusExA(sc_handle, SC_ENUM_PROCESS_INFO, types_to_get, SERVICE_STATE_ALL, NULL, 0, &bytes_to_get, &services_returned, &resume_handle, NULL);
    ENUM_SERVICE_STATUS_PROCESSA* service_statuses = (ENUM_SERVICE_STATUS_PROCESSA*)malloc(bytes_to_get);
    EnumServicesStatusExA(sc_handle, SC_ENUM_PROCESS_INFO, types_to_get, SERVICE_STATE_ALL, (LPBYTE)service_statuses, bytes_to_get, &bytes_to_get, &services_returned, &resume_handle, NULL);

    for (int a = 0; a < services_returned; a++)
    {
        auto service = info.add_service();

        service->set_id          (service_statuses[a].lpServiceName);
        service->set_display_name(service_statuses[a].lpDisplayName);

        switch (service_statuses[a].ServiceStatusProcess.dwCurrentState)
        {
        case SERVICE_START_PENDING: service->set_state(Bakaneko::Service_State_Starting); break;
        case SERVICE_RUNNING      : service->set_state(Bakaneko::Service_State_Running ); break;
        case SERVICE_STOP_PENDING : service->set_state(Bakaneko::Service_State_Stopping); break;
        case SERVICE_STOPPED      : service->set_state(Bakaneko::Service_State_Stopped ); break;
        }

        if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_WIN32) != 0)
            service->set_type("Process");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_KERNEL_DRIVER) != 0)
            service->set_type("Kernel Driver");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_FILE_SYSTEM_DRIVER) != 0)
            service->set_type("File System Driver");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_ADAPTER) != 0)
            service->set_type("Adapter");
        else if ((service_statuses[a].ServiceStatusProcess.dwServiceType & SERVICE_RECOGNIZER_DRIVER) != 0)
            service->set_type("Recongizer Driver");

        Win32ServiceHandle service_handle(OpenServiceA, sc_handle, service_statuses[a].lpServiceName, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);

        QueryServiceConfig2W(service_handle, SERVICE_CONFIG_DESCRIPTION, nullptr, 0, &bytes_to_get);
        SERVICE_DESCRIPTIONW* description = (SERVICE_DESCRIPTIONW*)malloc(bytes_to_get);
        memset(description, 0, bytes_to_get);
        QueryServiceConfig2W(service_handle, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)description, bytes_to_get, &bytes_to_get);

        if (description->lpDescription != nullptr)
            service->set_description(ljh::convert_string(description->lpDescription));

        free(description);

        QueryServiceConfigA(service_handle, nullptr, 0, &bytes_to_get);
        QUERY_SERVICE_CONFIGA* config = (QUERY_SERVICE_CONFIGA*)malloc(bytes_to_get);
        memset(config, 0, bytes_to_get);
        QueryServiceConfigA(service_handle, config, bytes_to_get, &bytes_to_get);
        
        switch (config->dwStartType)
        {
        case SERVICE_AUTO_START  :
        case SERVICE_BOOT_START  :
        case SERVICE_SYSTEM_START:
            service->set_enabled(true);
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
            for (auto& service : services)
            {
                auto service_interface = system_bus.get("org.freedesktop.systemd1", std::get<8>(service).data(), "org.freedesktop.systemd1.Unit");
                auto service_info = info.add_service();
                
                auto path = service_interface.get<std::string>("FragmentPath");
                if (!path.empty()) files.emplace(path);

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

                service_info->set_id(id);
                service_info->set_display_name(id);
                service_info->set_description(description);

                auto type = id.substr(id.find_last_of('.') + 1);
                type[0] = std::toupper(type[0]);
                service_info->set_type(type);

                auto enabled_state = service_interface.get<std::string>("UnitFileState");
                if (enabled_state == "enabled" || enabled_state == "static")
                    service_info->set_enabled(true);

                auto active_state = service_interface.get<std::string>("ActiveState");
                if (active_state == "inactive"    ) service_info->set_state(Bakaneko::Service_State_Stopped );
                if (active_state == "failed"      ) service_info->set_state(Bakaneko::Service_State_Stopped );
                if (active_state == "active"      ) service_info->set_state(Bakaneko::Service_State_Running );
                if (active_state == "activating"  ) service_info->set_state(Bakaneko::Service_State_Starting);
                if (active_state == "deactivating") service_info->set_state(Bakaneko::Service_State_Stopping);
            }
            auto unloaded = interface.call("ListUnitFiles").run<std::vector<std::tuple<std::string, std::string>>>();
            for (auto& service : unloaded)
            {
                auto id = std::get<0>(service);
                if (files.find(id) != files.end())
                    continue;
                if (id.find('@') != std::string::npos)
                    continue;

                id = id.substr(id.find_last_of('/') + 1);
                auto unit_path = interface.call("LoadUnit").args(id).run<ljh::unix::dbus::object_path>();
                
                auto service_interface = system_bus.get("org.freedesktop.systemd1", unit_path.data(), "org.freedesktop.systemd1.Unit");
                auto service_info = info.add_service();

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

                service_info->set_id(id);
                service_info->set_display_name(id);
                service_info->set_description(description);

                auto type = id.substr(id.find_last_of('.') + 1);
                type[0] = std::toupper(type[0]);
                service_info->set_type(type);

                auto enabled_state = service_interface.get<std::string>("UnitFileState");
                if (enabled_state == "enabled" || enabled_state == "static")
                    service_info->set_enabled(true);

                auto active_state = service_interface.get<std::string>("ActiveState");
                if (active_state == "inactive"    ) service_info->set_state(Bakaneko::Service_State_Stopped );
                if (active_state == "failed"      ) service_info->set_state(Bakaneko::Service_State_Stopped );
                if (active_state == "active"      ) service_info->set_state(Bakaneko::Service_State_Running );
                if (active_state == "activating"  ) service_info->set_state(Bakaneko::Service_State_Starting);
                if (active_state == "deactivating") service_info->set_state(Bakaneko::Service_State_Stopping);
            }
        }
        catch(const ljh::unix::dbus::error& e)
        {
            auto error_message = fmt::format("DBus Error: ({}) {}", e.name(), e.message());
            throw std::runtime_error{error_message};
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

ljh::expected<void, Errors> Control::Service(const Fields& fields, Bakaneko::Service_Control data)
{
    if (!fields.authentication.has_value())
        return ljh::unexpected{Errors::NeedsPassword};
    if (!Helpers::Authenticate(fields.authentication.value()))
        return ljh::unexpected{Errors::NeedsPassword};

    spdlog::debug("Action {} requsested on {}", data.action(), data.id());

#if defined(LJH_TARGET_Windows)
    Win32ServiceHandle sc_handle(OpenSCManagerA, nullptr, SERVICES_ACTIVE_DATABASE, GENERIC_READ);
    Win32ServiceHandle service_handle(OpenServiceA, sc_handle, data.id().c_str(), SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG);
    SERVICE_STATUS status;

    switch (data.action())
    {
    case Bakaneko::Service_Control_Action_Stop:
        if (ControlService(service_handle, SERVICE_CONTROL_STOP, &status) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service_Control_Action_Start:
        if (StartServiceA(service_handle, 0, nullptr) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service_Control_Action_Restart:
        if (ControlService(service_handle, SERVICE_CONTROL_STOP, &status) != 0)
            return ljh::unexpected{Errors::Failed};
        if (StartServiceA(service_handle, 0, nullptr) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service_Control_Action_Enable:
        if (ChangeServiceConfigA(service_handle, SERVICE_NO_CHANGE, SERVICE_AUTO_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    case Bakaneko::Service_Control_Action_Disable:
        if (ChangeServiceConfigA(service_handle, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != 0)
            return ljh::unexpected{Errors::Failed};
        break;
    }
#elif defined(LJH_TARGET_Linux)
    switch (get_service_manager())
    {
    case service_manager::systemd:
        try {
            bool _1;
            std::vector<std::tuple<std::string,std::string,std::string>> _2;

            ljh::unix::dbus::connection system_bus(ljh::unix::dbus::bus::SYSTEM);
            auto interface = system_bus.get("org.freedesktop.systemd1", "/org/freedesktop/systemd1", "org.freedesktop.systemd1.Manager");
            auto unit_path = interface.call("LoadUnit").args(data.id()).run<ljh::unix::dbus::object_path>();
            auto service_interface = system_bus.get("org.freedesktop.systemd1", unit_path.data(), "org.freedesktop.systemd1.Unit");
            switch (data.action())
            {
            case Bakaneko::Service_Control_Action_Stop:
                service_interface.call("Stop").args("replace").run<ljh::unix::dbus::object_path>();
                break;
            case Bakaneko::Service_Control_Action_Start:
                service_interface.call("Start").args("replace").run<ljh::unix::dbus::object_path>();
                break;
            case Bakaneko::Service_Control_Action_Restart:
                service_interface.call("Restart").args("replace").run<ljh::unix::dbus::object_path>();
                break;
            case Bakaneko::Service_Control_Action_Enable:
                {
                    auto path = service_interface.get<std::string>("FragmentPath");
                    if (path.empty()) return ljh::unexpected{Errors::Failed};
                    interface.call("EnableUnitFiles").args(std::vector{data.id()}, false, false).run(_1, _2);
                    for (auto& s : _2)
                    {
                        spdlog::debug("{}, {}, {}", std::get<0>(s), std::get<1>(s), std::get<2>(s));
                    }
                }
                break;
            case Bakaneko::Service_Control_Action_Disable:
                {
                    auto path = service_interface.get<std::string>("FragmentPath");
                    if (path.empty()) return ljh::unexpected{Errors::Failed};
                    interface.call("DisableUnitFiles").args(std::vector{data.id()}, false).run(_2);
                }
                break;
            }
        }
        catch(const ljh::unix::dbus::error& e)
        {
            auto error_message = fmt::format("DBus Error: ({}) {}", e.name(), e.message());
            throw std::runtime_error{error_message};
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