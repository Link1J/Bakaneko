// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "appinfo.h"
#include "bakaneko-version.h"

#include <KLocalizedString>

AppInfo::AppInfo()
{
}

AppInfo& AppInfo::Instance()
{
    static AppInfo instance;
    return instance;
}

constexpr auto app_id         = "bakaneko";
constexpr auto app_name       = "Bakaneko";
constexpr auto app_desciption = "A server management tool that has nothing to do with cats.";
constexpr auto app_copyright  = "© 2020 Jared Irwin";

KAboutData AppInfo::get_about()
{
    static auto about = []() {
        KAboutData about(app_id, i18n(app_name), BAKANEKO_VERSION_STRING);
        about.setShortDescription(i18n(app_desciption));
        about.setOrganizationDomain("link1j.me");
        about.setDesktopFileName("me.link1j.bakaneko.desktop");

        about.setLicense(KAboutLicense::GPL_V3, KAboutLicense::OrLaterVersions);
        about.setCopyrightStatement(i18n(app_copyright));

        about.addAuthor(i18n("Jared Irwin"), QString(), "jrairwin@sympatico.ca", "link1j.me");
        
        return about;
    }();
    return about;
}