// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.12
import QtQuick.Layouts 1.1
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Objects 1.0 as Objects
import Bakaneko.Pages 1.0 as Pages

RowLayout {
    id: roomLayout

    spacing: Kirigami.Units.largeSpacing
    width: listView.width

    TapHandler {
        onTapped: {
            currentServer = model.data
            currentServerIndex = model.index
            if (pageStack.depth < 2)
                pageStack.push(serverPage)
            else
                pageStack.replace(serverPage)
        }
    }

    Component {
        id: serverPage
        Pages.Server {}
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