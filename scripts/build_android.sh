#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

rm /output/*bakaneko*.apk

sudo cp /home/user/src/bakaneko/scripts/create-apk.sh /opt/helpers/create-apk
sudo cp /home/user/src/bakaneko/scripts/cp-with-prefix.sh /opt/helpers/cp-with-prefix

sudo chmod -R +x /opt/helpers

/opt/helpers/build-cmake bakaneko bakaneko -DCMAKE_BUILD_TYPE=Release `python3 /opt/helpers/get-apk-args.py /home/user/src/bakaneko`
/opt/helpers/create-apk bakaneko