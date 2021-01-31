// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <KAboutData>

class AppInfo : public QObject
{
    Q_OBJECT
    AppInfo();

public:
    static AppInfo& Instance();

private:
    Q_PROPERTY(KAboutData about READ get_about);
    Q_PROPERTY(QString icon_theme READ get_icon_theme CONSTANT);

public:

public Q_SLOTS:
    KAboutData get_about();
    QString get_icon_theme();

Q_SIGNALS:

private:
};

using AppInfoPointer = AppInfo*;
Q_DECLARE_METATYPE(AppInfoPointer);