# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

set(SRCS
    main.cpp
    models/serverlistmodel.cpp
    objects/server.cpp
    managers/servermanager.cpp
    managers/appinfo.cpp
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

target_link_libraries(bakaneko PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    
    KF5::I18n
    KF5::Crash
    KF5::Notifications
    KF5::Kirigami2
    KF5::CoreAddons
    KF5::IconThemes

    ssh
)
if(WIN32)
    target_link_libraries(bakaneko PRIVATE
        Iphlpapi
        ws2_32
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
    )
endif()

install(TARGETS bakaneko ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
