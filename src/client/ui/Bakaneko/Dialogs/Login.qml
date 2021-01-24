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

	header: Kirigami.Heading {
		text: i18n("Login")
	}
	contentItem: Kirigami.FormLayout {
		Controls.TextField {
			id: username
			Kirigami.FormData.label: i18n("Username")
			onAccepted: password.forceActiveFocus();
		}
		Controls.TextField {
			id: password
			Kirigami.FormData.label: i18n("Password")
			onAccepted: addButton.forceActiveFocus();
			echoMode: TextInput.PasswordEchoOnEdit
		}
		Controls.Button {
			id: addButton
			text: i18nc("@action:button", "Login")
			onClicked: {
				close()
			}
		}
	}
}