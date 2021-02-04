# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0091 NEW)
project(bakaneko)

include(FeatureSummary)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

find_package(ECM 5.68 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
#set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBUGINFO Release Debug)

include(KDEInstallDirs)
include(ECMSetupVersion)

ecm_setup_version(0.1.1
    VARIABLE_PREFIX BAKANEKO
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/bakaneko-version.h
)

add_subdirectory(externals)
add_subdirectory(src/protobuf)

set_target_properties(ljh spdlog protobuf-files PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

if (NOT BAKANEKO_DONT_BUILD_CLIENT)
    add_subdirectory(src/client)
    set_target_properties(bakaneko-client PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
endif()
if (NOT BAKANEKO_DONT_BUILD_SERVER)
    add_subdirectory(src/server)
    set_target_properties(bakaneko-server PROPERTIES MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
