# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

cmake_minimum_required(VERSION 3.16)
project(bakaneko)

include(FeatureSummary)

find_package(ECM 5.68 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
#set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBUGINFO Release Debug)

include(KDEInstallDirs)
include(ECMSetupVersion)

ecm_setup_version(0.1.0
    VARIABLE_PREFIX BAKANEKO
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/bakaneko-version.h
)

add_subdirectory(externals)
add_subdirectory(src/commun)

if (NOT BAKANEKO_DONT_BUILD_CLIENT)
    add_subdirectory(src/client)
endif()
if (NOT BAKANEKO_DONT_BUILD_SERVER)
    add_subdirectory(src/server)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
