// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"
#include "windows.hpp"

#include <filesystem>
#include <chrono>

#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>

#if defined(LJH_TARGET_Windows)
#include <windows.h>
#include <ljh/windows/wmi.hpp>
#endif

#include <spdlog/spdlog.h>

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
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    return ljh::expected<void, Errors>{};
}