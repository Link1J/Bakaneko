# Bakaneko
![Version](https://img.shields.io/badge/version-0.0.1-blue.svg?cacheSeconds=2592000)
[![License: GPL v3 or later](https://img.shields.io/badge/License-GPL%20v3%20or%20later-yellow.svg)](https://www.gnu.org/licenses/gpl-3.0)

> A server management tool that has nothing to do with cats.

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
craft kirigami ki18n kcrash knotifications kcoreaddons kiconthemes qqc2-desktop-style libssh
```
Then clone the repo,
```powershell
git clone https://github.com/link1j/bakaneko && cd bakaneko
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
git clone https://github.com/link1j/bakaneko && cd bakaneko
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
git clone https://github.com/link1j/bakaneko && cd bakaneko
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

Licenses for external libaries used are in [NOTICES.md](NOTICES.md)
