// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"
#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>

#undef interface
#include "server.pb.h"

extern std::tuple<int, std::string> exec(const std::string& cmd);

Http::Response Info::Updates(const Http::Request& request)
{
    Bakaneko::Updates updates;

#if defined(LJH_TARGET_Windows)
    return {501};
#elif defined(LJH_TARGET_Linux)
    auto run = [&updates](const std::string& command, const std::string& flags, bool(*decode)(Bakaneko::Update&, std::string)) -> bool {
        auto [exit_code, std_out] = exec(command + ' ' + flags);
        if (exit_code == 0)
        {
            auto packages = ljh::split(std_out, '\n');
            for (auto& package : packages)
            {
                if (package.empty())
                    continue;

                Bakaneko::Update update;
                if (decode(update, package))
                    updates.mutable_update()->Add(std::move(update));
            }

            return true;
        }
        return false;
    };

    auto pacman_decode = [](Bakaneko::Update& update, std::string package) {
        size_t space = package.find(' '), pre_space = 0;
        update.set_name(package.substr(pre_space, space - pre_space));

        pre_space = space;
        space = package.find(' ', space + 1);
        update.set_old_version(package.substr(pre_space, space - pre_space));

        pre_space = space + 4;
        update.set_new_version(package.substr(pre_space));

        return true;
    };
    auto apt_decode = [](Bakaneko::Update& update, std::string package) {
        auto info = ljh::split(package, ' ');
        if (info.size() != 6)
            return false;

        update.set_name       (info[0].substr(0, info[0].find('/')));
        update.set_old_version(info[5].substr(0, info[5].size()-1 ));
        update.set_new_version(info[1]                             );

        return true;
    };

    if (!run("yay", "-Qu", pacman_decode))
        run("pacman", "-Qu", pacman_decode);
    run("apt", "list --upgradable", apt_decode);
#endif

    return Http::output_message(updates, request);
}