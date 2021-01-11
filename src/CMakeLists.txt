# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

set(SRCS
    main.cpp
    models/serverlistmodel.cpp
    objects/server.cpp
    managers/servermanager.cpp
    managers/appinfo.cpp
    managers/settings.cpp
    term/pty.cpp
    term/screen.cpp
)
qt5_add_resources(RESOURCES
    qml.qrc
    resources.qrc
)
ecm_add_app_icon(APP_ICONS
    ICONS
        ../assets/64-me.link1j.bakaneko.png
        ../assets/256-me.link1j.bakaneko.png
)

add_executable(bakaneko ${SRCS} ${RESOURCES} ${APP_ICONS})

set_target_properties(bakaneko PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
    WIN32_EXECUTABLE TRUE
    MSVC_RUNTIME_LIBRARY MultiThreadedDLL
)
target_compile_options(bakaneko PRIVATE
    $<$<PLATFORM_ID:Linux,Android>:
        -fexceptions
    >
)

target_include_directories(bakaneko PRIVATE
    ${CMAKE_BINARY_DIR}
    ../externals/Yat/backend
    ../externals/Yat/qml/Yat/plugin
)

target_link_libraries(bakaneko PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg

    Qt5::QuickPrivate
    
    KF5::I18n
    KF5::Crash
    KF5::Notifications
    KF5::Kirigami2
    KF5::CoreAddons

    ssh
)
if(WIN32)
    target_link_libraries(bakaneko PRIVATE
        Iphlpapi
        ws2_32
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

install(TARGETS bakaneko ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
