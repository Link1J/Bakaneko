// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "info.hpp"
#include <ljh/system_info.hpp>
#include <ljh/string_utils.hpp>

#undef interface

#include <regex>

#if defined(LJH_TARGET_Windows)
#include <ljh/windows/com_bstr.hpp>
#include <wuapi.h>
#endif

extern std::tuple<int, std::string> exec(const std::string &cmd);

ljh::expected<Bakaneko::Updates, Errors> Info::Updates(const Fields &fields)
{
    decltype(Info::Updates(fields))::value_type updates;

#if defined(LJH_TARGET_Windows)
    using namespace ljh::windows::com_bstr_literals;
    winrt::com_ptr updates_session = winrt::create_instance<IUpdateSession>(CLSID_UpdateSession);
    winrt::com_ptr<IUpdateSearcher> searcher;
    winrt::check_hresult(updates_session->CreateUpdateSearcher(searcher.put()));
    winrt::com_ptr<ISearchResult> results;
    winrt::check_hresult(searcher->Search(L"( IsInstalled = 0 and IsHidden = 0 )"_bstr, results.put()));
    winrt::com_ptr<IUpdateCollection> update_list;
    winrt::check_hresult(results->get_Updates(update_list.put()));
    LONG update_size;
    winrt::check_hresult(update_list->get_Count(&update_size));
    for (LONG i = 0; i < update_size; i++)
    {
        winrt::com_ptr<IUpdate> update_item;
        winrt::check_hresult(update_list->get_Item(i, update_item.put()));

        auto update = updates.add_update();

        ljh::windows::com_bstr update_name;
        winrt::check_hresult(update_item->get_Title(update_name.put()));
        update.name = (ljh::convert_string(update_name));
    }
#elif defined(LJH_TARGET_Linux)
    auto run = [&updates](const std::string &command, const std::string &flags, bool (*decode)(Bakaneko::Update &, std::string)) -> bool {
        auto [exit_code, std_out] = exec(command + ' ' + flags);
        if (exit_code == 0)
        {
            auto packages = ljh::split(std_out, '\n');
            for (auto &package : packages)
            {
                if (package.empty())
                    continue;

                Bakaneko::Update update;
                if (decode(update, package))
                    updates.updates.emplace_back(std::move(update));
            }

            return true;
        }
        return false;
    };

    auto pacman_decode = [](Bakaneko::Update &update, std::string package) {
        size_t space = package.find(' '), pre_space = 0;
        update.name = (package.substr(pre_space, space - pre_space));

        pre_space = space;
        space = package.find(' ', space + 1);
        update.old_version = (package.substr(pre_space, space - pre_space));

        pre_space = space + 4;
        update.new_version = (package.substr(pre_space));

        return true;
    };
    auto apt_decode = [](Bakaneko::Update &update, std::string package) {
        auto info = ljh::split(package, ' ');
        if (info.size() != 6)
            return false;

        update.name = (info[0].substr(0, info[0].find('/')));
        update.old_version = (info[5].substr(0, info[5].size() - 1));
        update.new_version = (info[1]);

        return true;
    };
    auto apk_decode = [](Bakaneko::Update &update, std::string package) {
        auto info = ljh::split(package, '=');
        if (info.size() != 2)
            return false;

        std::regex version(R"((.*)-(\d.*?-r\d))");
        std::smatch sm;
        if (!std::regex_search(info[0], sm, version))
            return false;

        update.name = (sm[1].str());
        update.old_version = (sm[2].str());
        update.new_version = (info[1]);

        return true;
    };

    run("pacman", "-Qu", pacman_decode);
    run("apt", "list --upgradable", apt_decode);
    run("apk", "version -v -l '<'", apk_decode);
#else
    return ljh::unexpected{Errors::NotImplemented};
#endif

    return std::move(updates);
}