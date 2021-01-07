import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.15 as Controls
import ServerManager.Objects 1.0 as Objects

RowLayout {
    id: roomLayout
    spacing: Kirigami.Units.largeSpacing
    width: listView.width
    TapHandler {
        onTapped: {
            currentServer = model.data
            pageStack.push(serverpage)
        }
    }

    Kirigami.Icon {
        source: model.state == Objects.Server.State.Online ? "online" : "offline"
    }
    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Controls.Label {
            Layout.fillWidth: true
            text: model.hostname
        }
        RowLayout {
            Layout.fillWidth: true
            Controls.Label {
                text: model.ip
            }
        }
    }
}