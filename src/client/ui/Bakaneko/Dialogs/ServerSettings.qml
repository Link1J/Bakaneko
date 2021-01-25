// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Objects 1.0 as Objects
import Bakaneko.Managers 1.0 as Managers

Kirigami.OverlaySheet {
	id: addServerDialog

	parent: applicationWindow().overlay

	property int serverIndex: -1;
	property var server: -1;

	header: Kirigami.Heading {
		text: i18n("Server Settings")
	}
	contentItem: Kirigami.FormLayout {
		Controls.TextField {
			id: ip
			Kirigami.FormData.label: i18n("Server IP")
			text: server.ip
			onAccepted: addButton.forceActiveFocus();
		}
		Row {
			Controls.Button {
				id: addButton
				text: i18nc("@action:button", "Update")
				onClicked: {
					Managers.Server.AddServer(serverIndex, ip.text);
					close()
				}
			}
			Controls.Button {
				id: a
				text: i18nc("@action:button", "Cancel")
				onClicked: {
					close()
				}
			}
		}
	}
}