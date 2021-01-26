# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

find_package(Protobuf REQUIRED)
find_package(Boost 1.66.0 REQUIRED)
find_package(OpenSSL REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    ../protobuf/server.proto
    ../protobuf/updates.proto
    ../protobuf/drives.proto
    ../protobuf/network.proto
)

set(SRCS
)

add_library(commun STATIC ${SRCS} ${PROTO_SRCS} ${PROTO_HDRS})

set_target_properties(commun PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS OFF
    MSVC_RUNTIME_LIBRARY MultiThreadedDLL
)
target_compile_options(commun PUBLIC
    -D_WIN32_WINNT=0x0601
)

target_include_directories(commun PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(commun PUBLIC
    ljh
    protobuf::libprotobuf
    Boost::boost
    OpenSSL::SSL
)