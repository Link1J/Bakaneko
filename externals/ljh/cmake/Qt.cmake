
#          Copyright Jared Irwin 2020-2021
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

# Qt.cmake - v1.0
# SPDX-License-Identifier: BSL-1.0
#
# Requires CMake
#
# Based on code from https://stackoverflow.com/questions/18658233/split-string-to-3-variables-in-cmake
# Based on code from https://stackoverflow.com/questions/15639781/how-to-find-the-qt5-cmake-module-on-windows
#
# ABOUT
#
# USAGE
#
# Version History
#     1.0 Inital Version

set(QT_MISSING True)
if (MSVC)
    if (NOT QT_VERSION_FIND)
        set(QT_VERSION_FIND "*.*.*")
    endif()

    get_filename_component(QT_BIN [HKEY_CURRENT_USER\\Software\\Classes\\Applications\\QtProject.QtCreator.cpp\\shell\\Open\\Command] PATH)

    string(REPLACE "/Tools" ";" QT_BIN "${QT_BIN}")
    list(GET QT_BIN 0 QT_BIN)
    file(GLOB QT_VERSIONS "${QT_BIN}${QT_VERSION_FIND}")
    list(SORT QT_VERSIONS)

    list(REVERSE QT_VERSIONS)
    list(GET QT_VERSIONS 0 QT_PATH)

    string(REPLACE "//" "/"  QT_PATH "${QT_PATH}")

    string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" QT_VERSION ${QT_PATH})
    string(REPLACE "." ";" QT_VERSION_LIST ${QT_VERSION})
    list(GET QT_VERSION_LIST 0 QT_VERSION_MAJOR)
    list(GET QT_VERSION_LIST 1 QT_VERSION_MINOR)
    list(GET QT_VERSION_LIST 2 QT_VERSION_PATCH)

    if ((MSVC_VERSION GREATER_EQUAL "1900"))
        file(GLOB QT_COMPILERS "${QT_PATH}/msvc*")
        list(REVERSE QT_COMPILERS)
        list(GET QT_COMPILERS 0 QT_MSVC_PATH)
        string(REGEX MATCH "[0-9][0-9][0-9][0-9]+" QT_MSVC ${QT_MSVC_PATH})
    else()
        MATH(EXPR QT_MSVC "2000 + (${MSVC_VERSION} - 500) / 100")
    endif()

    if (CMAKE_SYSTEM_PROCESSOR MATCHES 64)
        set(QT_MSVC "${QT_MSVC}_64")
    endif()

    set(QT_PATH "${QT_PATH}/msvc${QT_MSVC}/")

    if (IS_DIRECTORY ${QT_PATH})
        set(QT_MISSING False)
    endif()
endif()

if (NOT QT_MISSING)
    set(QT_PREFIX ${QT_PATH}/lib/cmake/)
    message("-- Qt found: ${QT_VERSION} ${QT_PATH}")

    if (WIN32 AND EXISTS "${QT_PATH}bin/windeployqt.exe")
        message("-- windeployqt found: ${QT_PATH}bin/windeployqt.exe")
        add_executable(Qt::windeployqt IMPORTED)
        set_target_properties(Qt::windeployqt PROPERTIES IMPORTED_LOCATION "${QT_PATH}bin/windeployqt.exe")
    endif()
else()
    if (NOT MSVC)
        message(SEND_ERROR "-- Qt not found")
    endif()
endif()