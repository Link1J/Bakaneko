# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

cmake_minimum_required(VERSION 3.16)
project(bakaneko)

include(FeatureSummary)

find_package(ECM 5.75 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)

add_subdirectory(externals)

if (NOT BAKANEKO_DONT_BUILD_CLIENT)
    add_subdirectory(src/client)
endif()
if (NOT BAKANEKO_DONT_BUILD_SERVER)
    add_subdirectory(src/server)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
