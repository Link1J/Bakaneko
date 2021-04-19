# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

cmake_policy(SET CMP0077 NEW)

set(ljh_INSTALL OFF CACHE BOOL "")
add_subdirectory(ljh)

add_subdirectory(spdlog)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)