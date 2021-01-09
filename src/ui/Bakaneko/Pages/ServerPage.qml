// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Objects 1.0 as Objects

Kirigami.Page {
    id: page
    title: currentServer.hostname

    // actions.left: Kirigami.Action {
    //     id: powerStateChange
    //     icon.name: "system-shutdown"
    //     text: currentServer.state == Objects.Server.State.Online ? i18n("Shutdown") : i18n("Power On")
    //     onTriggered: {
    //         if (currentServer.state == Objects.Server.State.Online) {
    //             currentServer.shutdown();
    //         }
    //         else {
    //             currentServer.wake_up();
    //         }
    //     }
    // }

    actions.contextualActions: [
        Kirigami.Action {
            icon.name: "system-shutdown"
            text: currentServer.state == Objects.Server.State.Online ? i18n("Shutdown") : i18n("Power On")
            onTriggered: {
                if (currentServer.state == Objects.Server.State.Online) {
                    currentServer.shutdown();
                }
                else {
                    currentServer.wake_up();
                }
            }
        },
        Kirigami.Action {
            icon.name: "system-reboot"
            text: i18n("Reboot")
            enabled: currentServer.state == Objects.Server.State.Online
            onTriggered: {
                currentServer.reboot();
            }
        }
    ]

    
    ColumnLayout {
        anchors.fill: parent
        
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true

            FontMetrics {
                id: fontMetrics
            }

            Kirigami.Icon {
                readonly property var max_height: 6 * (temp.height + Kirigami.Units.smallSpacing) + Kirigami.Units.smallSpacing - 1
                readonly property var item_width: icon.width + Kirigami.Units.largeSpacing + icon.x
                readonly property var page_width: page.width - page.leftPadding - page.rightPadding * 2

                id: icon

                Layout.alignment: Qt.AlignTop | Qt.AlignRight
                Layout.rightMargin: Kirigami.Units.largeSpacing

                source: currentServer.system_icon
                implicitHeight:
                    page_width - info.x < info.width
                    ? page_width - info.width < max_height
                        ? page_width - info.width
                        : max_height
                    : max_height
                implicitWidth: icon.implicitHeight
            }

            Kirigami.FormLayout {
                id: info
            
                Layout.fillWidth: false
                Layout.alignment: Qt.AlignTop

                wideMode: true

                Controls.Label {
                    id: temp
                    Kirigami.FormData.label: "OS:"
                    text: currentServer.os
                }
                Controls.Label {
                    Kirigami.FormData.label: "Kernel:"
                    text: currentServer.kernel
                }
                Controls.Label {
                    Kirigami.FormData.label: "Architecture:"
                    text: currentServer.arch
                }
                Controls.Label {
                    Kirigami.FormData.label: "VM Platform:"
                    text: currentServer.vm_platform
                    visible: currentServer.vm_platform != ""
                }
                Controls.Label {
                    Kirigami.FormData.label: "IP:"
                    text: currentServer.ip
                }
                Controls.Label {
                    Kirigami.FormData.label: "MAC:"
                    text: currentServer.mac
                }
            }
        }
        Kirigami.Separator {
        }
    }
}