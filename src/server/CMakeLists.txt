# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

set(SRCS
    main.cpp
    rest.cpp
    drives.cpp
    system.cpp
    updates.cpp
    windows_service.cpp
    network.cpp
    info.cpp
    services.cpp
)

add_executable(bakaneko-server ${SRCS})

set_target_properties(bakaneko-server PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
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
    protobuf-files
    spdlog::spdlog
)
if (WIN32)
    target_link_libraries(bakaneko-server PUBLIC
        IPHLPAPI.lib
    )
endif()

install(TARGETS bakaneko-server ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
