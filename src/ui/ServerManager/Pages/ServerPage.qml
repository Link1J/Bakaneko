import QtQuick 2.1
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.0 as Controls
import ServerManager.Objects 1.0 as Objects

Kirigami.Page {
    title: currentServer.hostname

    actions.left: Kirigami.Action {
        id: power_state_change
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
    }

    GridLayout {
        columns: 2
        Kirigami.Icon {
            Layout.minimumWidth : Kirigami.Units.iconSizes.small
            Layout.minimumHeight: Kirigami.Units.iconSizes.small
            Layout.maximumWidth : Kirigami.Units.iconSizes.huge
            Layout.maximumHeight: Kirigami.Units.iconSizes.huge
            Layout.alignment: Qt.AlignTop

            source: currentServer.system_icon
        }
        GridLayout {
            implicitWidth: os_label.implicitWidth + kernel.implicitWidth
            Layout.fillWidth: true
            columns: 2
            
            Controls.Label {
                id: os_label
                text: "OS"
            }
            Controls.Label {
                Layout.fillWidth: true
                text: ": " + currentServer.os
            }
            Controls.Label {
                text: "Kernel"
            }
            Controls.Label {
                id: kernel
                Layout.fillWidth: true
                text: ": " + currentServer.kernel
            }
            Controls.Label {
                text: "Architecture"
            }
            Controls.Label {
                Layout.fillWidth: true
                text: ": " + currentServer.arch
            }
            Controls.Label {
                visible: currentServer.vm_platform != ""
                text: "VM Platform"
            }
            Controls.Label {
                Layout.fillWidth: true
                visible: currentServer.vm_platform != ""
                text: ": " + currentServer.vm_platform
            }
            Controls.Label {
                text: "IP"
            }
            Controls.Label {
                Layout.fillWidth: true
                text: ": " + currentServer.ip
            }
            Controls.Label {
                text: "MAC"
            }
            Controls.Label {
                Layout.fillWidth: true
                text: ": " + currentServer.mac
            }
        }
    }
}