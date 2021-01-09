#!/bin/bash

# From https://phabricator.kde.org/source/sysadmin-ci-tooling/browse/master/system-images/android/sdk/create-apk;3a8b4521abfd64d957559e38414084f45f390ba0
# This file does not use the GPL-3.0-or-later licence

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PATH=$DIR:$PATH

package=$1
shift

#Replace versionCode with seconds from epoch
find src/$package -name AndroidManifest.xml -exec sed -i s/'android:versionCode="[0-9]*"'/android:versionCode=\"$(date +%s)\"/ {} \;

function create_apk
{
    pushd build-$1/$package

    if [ -f /keys/${package}keystore-config ]; then
        ARGS="`cat /keys/${package}keystore-config` $ARGS" make create-apk
    elif [ -f /keys/androidkeystore-config ]; then
        ARGS="`cat /keys/androidkeystore-config` $ARGS" make create-apk
    else
        make create-apk
    fi

    if [ -d /output ]
    then
        find *_build_apk/build/outputs/apk/ -name \*.apk -exec cp-with-prefix "$1" {} /output/ \;
    fi
    popd
}

if [[ ! $ONLY_ARM64 ]]; then
    create_apk arm
fi

if [[ ! $ONLY_ARM32 ]]; then
    create_apk arm64
fi