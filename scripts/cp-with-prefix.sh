#!/bin/bash

# From https://phabricator.kde.org/source/sysadmin-ci-tooling/browse/master/system-images/android/sdk/;3a8b4521abfd64d957559e38414084f45f390ba0
# This file does not use the GPL-3.0-or-later licence

set -e

prefix=$1
shift

dest="${@: -1}"
array="${@:1:${#}-1}"

for v in $array;
do
    filename=$(basename $v)
    cp $v $dest/$prefix-$filename
done