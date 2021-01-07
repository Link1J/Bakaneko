#!/bin/bash
sudo wget https://phabricator.kde.org/file/download/ua6bgmnwiioe4w5kjpwk/PHID-FILE-67degldvu5ag4gbnoejc/create-apk -O /opt/helpers/create-apk
sudo wget https://phabricator.kde.org/file/download/ucmk522zm5qevkk3yph5/PHID-FILE-iryvm5hrlxuhorf6ovc7/cp-with-prefix -O /opt/helpers/cp-with-prefix

sudo chmod +x /opt/helpers/*

sudo apt install -y python3-pip
python3 -m pip install requests

/opt/helpers/build-kde-dependencies kirigami kcoreaddons ki18n kcrash knotifications kiconthemes

/opt/helpers/build-kde-project kirigami Frameworks -DBUILD_TESTING=OFF
/opt/helpers/build-kde-project kcoreaddons Frameworks -DBUILD_TESTING=OFF
/opt/helpers/build-kde-project ki18n Frameworks -DBUILD_TESTING=OFF
/opt/helpers/build-kde-project kcrash Frameworks -DBUILD_TESTING=OFF
/opt/helpers/build-kde-project knotifications Frameworks -DBUILD_TESTING=OFF
/opt/helpers/build-kde-project kiconthemes Frameworks -DBUILD_TESTING=OFF

/opt/helpers/build-cmake libssh https://git.libssh.org/projects/libssh.git -DWITH_INTERNAL_DOC=OFF -DWITH_TESTING=OFF -DWITH_SERVER=OFF -DWITH_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release 

/opt/helpers/build-cmake bakaneko bakaneko -DCMAKE_BUILD_TYPE=Release `python3 /opt/helpers/get-apk-args.py /home/user/src/bakaneko`
/opt/helpers/create-apk bakaneko