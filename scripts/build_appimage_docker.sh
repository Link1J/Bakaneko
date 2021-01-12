#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

# To use:
# docker run -ti --rm -v ${PWD}/appimages:/output -v ${PWD}:/home/user/src bakaneko-appimage bash /home/user/src/scripts/build_appimage_docker.sh

rm /output/Bakaneko*.AppImage 

cmake -B build -S src -G Ninja -DCMAKE_INSTALL_PREFIX=/usr
ninja -C build
DESTDIR=../appdir ninja -C build install
QML_SOURCES_PATHS=src/src ./linuxdeploy-x86_64.AppImage --appdir appdir --plugin qt --output appimage --appimage-extract-and-run
cp Bakaneko*.AppImage /output 

ninja -C build && DESTDIR=../appdir ninja -C build install && QML_SOURCES_PATHS=src/src ./linuxdeploy-x86_64.AppImage --appdir appdir --plugin qt --output appimage --appimage-extract-and-run