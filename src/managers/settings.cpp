// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#include "settings.h"

#include <QFontDatabase>

Settings::Settings()
{
    settings.beginGroup("settings");
}

Settings& Settings::Instance()
{
    static Settings settings;
    return settings;
}

uint16_t Settings::get_wol_port()
{
    settings.beginGroup("wol");
    auto value = settings.value("port", 9);
    settings.endGroup();
    return value.value<uint16_t>();
}

void Settings::set_wol_port(uint16_t value)
{
    settings.beginGroup("wol");
    settings.setValue("port", value);
    settings.endGroup();
    Q_EMIT changed_wol_port();
}

double Settings::get_server_refresh_rate()
{
    settings.beginGroup("server");
    auto value = settings.value("refresh_rate", 5.0);
    settings.endGroup();
    return value.value<double>();
}

void Settings::set_server_refresh_rate(double value)
{
    settings.beginGroup("server");
    settings.setValue("refresh_rate", value);
    settings.endGroup();
    Q_EMIT changed_wol_port();
}

QFont Settings::get_font()
{
    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    settings.beginGroup("term");
    auto value = settings.value("font", font);
    settings.endGroup();
    return value.value<QFont>();
}

void Settings::set_font(QFont value)
{
    settings.beginGroup("term");
    settings.setValue("font", value);
    settings.endGroup();
    Q_EMIT changed_font();
}