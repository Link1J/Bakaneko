# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

set(QT5_MIN_VERSION 5.14)
set(KF5_MIN_VERSION 5.75)

include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMEnableSanitizers)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 ${QT5_MIN_VERSION} REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Svg Concurrent)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 I18n Crash Notifications CoreAddons IconThemes Config)
find_package(libssh REQUIRED)

set(SRCS
    main.cpp
    models/serverlistmodel.cpp
    objects/server.cpp
    managers/servermanager.cpp
    managers/appinfo.cpp
    managers/settings.cpp
    term/pty.cpp
    term/screen.cpp
    term/terms/dumb.cpp
    term/term.cpp
    term/terms/linux.cpp
    managers/terminfo.cpp
    term/terms/null.cpp
    models/termlist.cpp
    models/updatemodel.cpp
    objects/servers/windows.cpp
    objects/servers/linux.cpp
    objects/update.cpp
    objects/drives.cpp
)
qt5_add_resources(RESOURCES
    qml.qrc
    resources.qrc
)
ecm_add_app_icon(APP_ICONS
    ICONS
        ../../assets/64-me.link1j.bakaneko.png
        ../../assets/128-me.link1j.bakaneko.png
        ../../assets/256-me.link1j.bakaneko.png
)

add_executable(client ${SRCS} ${RESOURCES} ${APP_ICONS})

set_target_properties(client PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
    WIN32_EXECUTABLE TRUE
    MSVC_RUNTIME_LIBRARY MultiThreadedDLL
)
target_compile_options(client PUBLIC
    $<$<PLATFORM_ID:Linux,Android>:
        -fexceptions
    >
    -DQT_NO_FOREACH
    -DQT_NO_KEYWORDS
    -DNOMINMAX
)

target_include_directories(client PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(client PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    Qt5::Concurrent
    
    KF5::I18n
    KF5::Crash
    KF5::Notifications
    KF5::Kirigami2
    KF5::CoreAddons
    
    KF5::ConfigCore
    KF5::ConfigGui

    ssh
    ljh
)
if(WIN32)
    target_link_libraries(client PRIVATE
        Iphlpapi
        ws2_32
        KF5::IconThemes
    )
endif()
if(APPLE)
    target_link_libraries(client PRIVATE
        KF5::IconThemes
    )
endif()
if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        list-add
        list-remove
        help-about
        online
        offline
        system-shutdown
        computer
        computer-laptop
        mail-send
        globe
        settings-configure
        system-reboot
        utilities-terminal
        computer-fail-symbolic
    )
endif()

install(TARGETS client ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS ../../assets/me.link1j.bakaneko.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES ../../assets/me.link1j.bakaneko.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES ../../assets/me.link1j.bakaneko.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
install(FILES bakaneko.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})
