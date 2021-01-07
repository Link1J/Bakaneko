# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

cmake_minimum_required(VERSION 3.17)
project(bakaneko)

include(FeatureSummary)

set(QT5_MIN_VERSION 5.12)
set(KF5_MIN_VERSION 5.65)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMPoQmTools)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMEnableSanitizers)

ecm_setup_version(0.0.1
    VARIABLE_PREFIX BAKANEKO
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/bakaneko-version.h
)

find_package(Qt5 ${QT5_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml QuickControls2 Svg)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 I18n Crash Notifications CoreAddons IconThemes)
find_package(libssh REQUIRED)

add_definitions(-DQT_NO_FOREACH -DQT_NO_KEYWORDS)
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(src)

install(PROGRAMS assets/me.link1j.bakaneko.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES assets/me.link1j.bakaneko.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES assets/me.link1j.bakaneko.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CPACK_PACKAGE_NAME "Server Manager")
set(CPACK_PACKAGE_NAME "Link1J")

