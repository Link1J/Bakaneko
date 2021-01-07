// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

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
    Q_PROPERTY(KAboutData about READ get_about)

public:

public Q_SLOTS:
    Q_INVOKABLE KAboutData get_about();

Q_SIGNALS:

private:
};

using AppInfoPointer = AppInfo*;
Q_DECLARE_METATYPE(AppInfoPointer);