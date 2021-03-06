// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QIcon>
#include <QDebug>
#include <QQuickStyle>
#include <QFontDatabase>
#include <QSplashScreen>

#include <KLocalizedContext>
#include <KCrash>

#include <iostream>
#include <string>

#ifdef Q_OS_WINDOWS
#include <winsock2.h>
#endif

#include "models/serverlistmodel.h"
#include "models/termlist.h"
#include "models/updatemodel.h"
#include "models/services.h"

#include "objects/server.h"
#include "objects/logindata.h"

#include "managers/servermanager.h"
#include "managers/appinfo.h"
#include "managers/settings.h"
#include "managers/terminfo.h"

#include "term/screen.h"

#include "base64.hpp"

Q_DECL_EXPORT int main(int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");
#else
    QApplication app(argc, argv);
    QPixmap pixmap(":/bakaneko-splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
    {
        QQuickStyle::setStyle("org.kde.desktop");
        //QQuickStyle::setStyle("org.kde.breeze");
    }
#endif

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/icons");

    QCoreApplication::setOrganizationName("Link1J");
    KAboutData::setApplicationData(AppInfo::Instance().get_about());
    QApplication::setWindowIcon(QIcon::fromTheme("me.link1j.bakaneko"));

    KCrash::initialize();

#ifdef Q_OS_WINDOWS
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

    qRegisterMetaType<Bakaneko::System     >("Bakaneko::System"     );
    qRegisterMetaType<Bakaneko::Drives     >("Bakaneko::Drives"     );
    qRegisterMetaType<Bakaneko::Updates    >("Bakaneko::Updates"    );
    qRegisterMetaType<Bakaneko::Adapters   >("Bakaneko::Adapters"   );
    qRegisterMetaType<Bakaneko::ServiceInfo>("Bakaneko::ServiceInfo");
    qRegisterMetaType<Bakaneko::Services   >("Bakaneko::Services"   );

    ServerManager::Instance().update_server_info(true);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);

#ifndef Q_OS_ANDROID
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &splash, &QSplashScreen::close);
#endif

    engine.addImportPath("qrc:/ui");

    qmlRegisterType             <ServerListModel>("Bakaneko.Models"    , 1, 0, "ServerList"                            );
    qmlRegisterType             <TermList       >("Bakaneko.Models"    , 1, 0, "TermList"                              );
    qmlRegisterType             <UpdateModel    >("Bakaneko.Models"    , 1, 0, "Updates"                               );
    qmlRegisterType             <Screen         >("Bakaneko.Components", 1, 0, "Screen"                                );
    qmlRegisterType             <LoginData      >("Bakaneko.Objects"   , 1, 0, "LoginData"                             );
    qmlRegisterUncreatableType  <Server         >("Bakaneko.Objects"   , 1, 0, "Server"    , ""                        );
    qmlRegisterSingletonInstance                 ("Bakaneko.Managers"  , 1, 0, "Server"    , &ServerManager::Instance());
    qmlRegisterSingletonInstance                 ("Bakaneko.Managers"  , 1, 0, "AppInfo"   , &AppInfo      ::Instance());
    qmlRegisterSingletonInstance                 ("Bakaneko.Managers"  , 1, 0, "Settings"  , &Settings     ::Instance());
    qmlRegisterSingletonInstance                 ("Bakaneko.Managers"  , 1, 0, "TermInfo"  , &TermInfo     ::Instance());

    engine.load(QUrl(QStringLiteral("qrc:/ui/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
        
    ServerManager::Instance().start();

    auto exit_code = app.exec();

#ifdef Q_OS_WINDOWS
    WSACleanup();
#endif

    return exit_code;
}
