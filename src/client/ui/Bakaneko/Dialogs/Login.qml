// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.9 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Objects 1.0 as Objects
import Bakaneko.Managers 1.0 as Managers

Kirigami.OverlaySheet {
	id: addServerDialog

	parent: applicationWindow().overlay

	property var source_page

	signal run(Objects.LoginData logindata)

	header: Kirigami.Heading {
		text: i18n("Login")
	}
	contentItem: ColumnLayout {
		Objects.LoginData {
			id: logindata
			page    : source_page;
			login   : addServerDialog;
			username: username.text;
			password: password.text;
		}
		Kirigami.InlineMessage {
			Layout.fillWidth: true
			id: fail_text
			type: Kirigami.MessageType.Error
			text: "Something fucked up. Twice. Because you shouldn't be seeing this message."
		}
		Controls.BusyIndicator {
			Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
			id: indic
			running: false
			visible: false
		}
		Kirigami.FormLayout {
			Layout.fillWidth: true
			id: form
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
			Row {
				Controls.Button {
					id: addButton
					text: i18nc("@action:button", "Login")
					onClicked: {
						if (username.text !== "" && password.text !== "") {
							form     .visible = false;
							addButton.enabled = false;
							indic.running     = true ;
							indic.visible     = true ;
							fail_text.visible = false;
							run(logindata)
						}
						else if (username.text === "") {
							fail_text.text = "No username given";
							fail_text.visible = true;
						}
						else if (password.text === "") {
							fail_text.text = "No password given";
							fail_text.visible = true;
						}
					}
				}
			}
		}
	}
	function connecting_fail(msg) {
		fail_text.text    = msg;
		fail_text.visible = true;
		form     .visible = true;
		addButton.enabled = true;
		indic.running     = false;
		indic.visible     = false;
	}
	function done() {
		close()
	}
}