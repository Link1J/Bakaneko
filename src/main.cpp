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

#include <KLocalizedContext>
#include <KCrash>

#include <iostream>
#include <string>
#include <filesystem>

#ifdef Q_OS_WINDOWS
#include <winsock2.h>
#endif

#include "models/serverlistmodel.h"

#include "objects/server.h"

#include "managers/servermanager.h"
#include "managers/appinfo.h"
#include "managers/settings.h"
#include "managers/terminfo.h"

#include "term/screen.h"

// #if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
template<typename T>
inline QObject* _i_helper(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    return &T::Instance();
}

template<typename T>
inline auto qmlRegisterSingletonInstance_temp(const char* uri, int versionMajor, int versionMinor, const char* typeName, T* cppObject)
{
    return qmlRegisterSingletonType<T>(uri, versionMajor, versionMinor, typeName, _i_helper<T>);
}
// #endif

Q_DECL_EXPORT int main(int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");
#else
    QApplication app(argc, argv);
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
        QQuickStyle::setStyle("org.kde.desktop");
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

    QQmlApplicationEngine engine;

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    engine.rootContext()->setContextProperty("fixedFont", fixedFont);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);

    engine.addImportPath("qrc:/ui");

    qmlRegisterType             <ServerListModel>("Bakaneko.Models"    , 1, 0, "ServerList"                            );
    qmlRegisterType             <Screen         >("Bakaneko.Components", 1, 0, "Screen"                                );
    qmlRegisterUncreatableType  <Server         >("Bakaneko.Objects"   , 1, 0, "Server"    , ""                        );
    qmlRegisterSingletonInstance_temp                 ("Bakaneko.Managers"  , 1, 0, "Server"    , &ServerManager::Instance());
    qmlRegisterSingletonInstance_temp                 ("Bakaneko.Managers"  , 1, 0, "AppInfo"   , &AppInfo      ::Instance());
    qmlRegisterSingletonInstance_temp                 ("Bakaneko.Managers"  , 1, 0, "Settings"  , &Settings     ::Instance());
    qmlRegisterSingletonInstance_temp                 ("Bakaneko.Managers"  , 1, 0, "TermInfo"  , &TermInfo     ::Instance());

    engine.load(QUrl(QStringLiteral("qrc:/ui/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    auto exit_code = app.exec();

#ifdef Q_OS_WINDOWS
    WSACleanup();
#endif

    return exit_code;
}