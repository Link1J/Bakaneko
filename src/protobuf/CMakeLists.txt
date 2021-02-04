# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

find_package(Protobuf REQUIRED)
find_package(Boost 1.66.0 REQUIRED)
find_package(OpenSSL REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    server.proto
    updates.proto
    drives.proto
    network.proto
    services.proto
)

set(SRCS
)

add_library(protobuf-files STATIC ${SRCS} ${PROTO_SRCS} ${PROTO_HDRS})

set_target_properties(protobuf-files PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
    MSVC_RUNTIME_LIBRARY MultiThreadedDLL
)
target_compile_options(protobuf-files PUBLIC
    -D_WIN32_WINNT=0x0601
)

target_include_directories(protobuf-files PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf-files PUBLIC
    ljh
    protobuf::libprotobuf
    Boost::boost
    OpenSSL::SSL
)