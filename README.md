# Bakaneko
[![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/link1j/Bakaneko?sort=semver)](https://github.com/Link1J/Bakaneko/releases/latest)
[![GitHub](https://img.shields.io/github/license/link1j/Bakaneko)](https://www.gnu.org/licenses/gpl-3.0)

> A server management tool that has nothing to do with cats.

## Build Status
#### Server
| Build Type | Status|
|---|---|
| Linux (GLibc) | [![Build Status](https://ci.link1j.me/badges/bakaneko-server-linux_glibc.svg)](https://ci.link1j.me/#/builders/3) |

#### Client
| Build Type | Status|
|---|---|
| Linux (GLibc) | [![Build Status](https://ci.link1j.me/badges/bakaneko-client-linux_glibc.svg)](https://ci.link1j.me/#/builders/4) |

## Building
### Windows
#### Requirements
- [Craft](https://community.kde.org/Craft)
- [Visual Studio](https://visualstudio.microsoft.com/) 2019 
- [cmake](https://cmake.org/)

> Craft has a guide for installation [here](https://community.kde.org/Guidelines_and_HOWTOs/Build_from_source/Windows). It is used to provide the KDE libaries on Windows.

> MinGW is not supported, and may never be supported.

#### Instructions
From a Craft prompt, run the command bellow to install the needed libaries.
```powershell
craft kirigami ki18n kcrash knotifications kcoreaddons kiconthemes qqc2-desktop-style libssh boost-system protobuf
```
Then clone the repo,
```powershell
git clone --recursive https://github.com/link1j/bakaneko && cd bakaneko
```
And finally build
```powershell
mkdir build && cd build
cmake ..
```

### Linux
#### Requirements
- A C++ compiler with C++17 support
- [cmake](https://cmake.org/) >= 3.16
- [Qt](https://www.qt.io/) >= 5.14 < 6.0.0
  - Core
  - Gui
  - Qml
  - QuickControls2
  - Svg
- [git](https://git-scm.com/)
- KDE Frameworks >= 5.75.0
  - Kirigami
  - KI18n
  - KCrash
  - KCoreAddons
  - KIconThemes
- Extra CMake Modules
> All of these can be installed by your package manager. 
> If not follow the Windows guide.

#### Instructions
```bash
git clone --recursive https://github.com/link1j/bakaneko && cd bakaneko
mkdir build && cd build
cmake ..
```

### Android
#### Requirements
- [docker](https://www.docker.com/git)
- [git](https://git-scm.com/)

#### Instructions
First clone the repo
```bash
git clone --recursive https://github.com/link1j/bakaneko && cd bakaneko
```
Build the docker image
```bash
docker build -f "scripts/Dockerfile.android" -t bakaneko-android:latest "scripts"
```
Then run docker
```bash
docker run -ti --rm -v ${PWD}/apks:/output -v ${PWD}:/home/user/src/bakaneko bakaneko-android bash /home/user/src/bakaneko/scripts/build_android_docker.sh
```


## Contributing
Please refer to our [Contributing Guide](CONTRIBUTING.md) for more details.

## Author

👤 **Jared Irwin**

* Website: link1j.me
* Github: [@Link1J](https://github.com/Link1J)
* LinkedIn: [@jaredirwin](https://linkedin.com/in/jaredirwin)

## Show your support

Give a ⭐️ if this project helped you!

## 📝 License

Copyright © 2020 [Jared Irwin](https://github.com/Link1J).

This project is [GPL v3 or later](LICENSE.md) licensed.

Some files are also licensed under [Boost Software License 1.0](LICENSE_1_0.txt), as to 
allow the file's use in external projects without forcing GPL onto them. Files with the 
dual GPLv3/BSL-1.0 will say so at the top.

Licenses for external libaries used are in [NOTICES.md](NOTICES.md)
