# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

set(SRCS
    main.cpp
    rest.cpp
    drives.cpp
    system.cpp
    updates.cpp
    windows_service.cpp
)

add_executable(bakaneko-server ${SRCS})

set_target_properties(bakaneko-server PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
    # MSVC_RUNTIME_LIBRARY MultiThreadedDLL
)
target_compile_options(bakaneko-server PUBLIC
    $<$<PLATFORM_ID:Windows>:
        -D_WIN32_WINNT=0x0601
    >
)

target_include_directories(bakaneko-server PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(bakaneko-server PUBLIC
    ljh
    commun
    spdlog::spdlog
)

install(TARGETS bakaneko-server ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if (UNIX)
    install(FILES "${CMAKE_SOURCE_DIR}/extras/bakaneko-server.service" DESTINATION "${KDE_INSTALL_FULL_SYSTEMDUNITDIR}/system")
endif()