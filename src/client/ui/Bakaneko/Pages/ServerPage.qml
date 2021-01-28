// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.4
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.9 as Controls
import Bakaneko.Objects 1.0 as Objects
import Bakaneko.Pages 1.0 as Pages
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components
import Bakaneko.Dialogs 1.0 as Dialogs

Kirigami.Page {
	id: page
	title: currentServer.hostname
	padding: 0

	actions.contextualActions: [
		Kirigami.Action {
			visible: !Kirigami.Settings.isMobile
			icon.name: "utilities-terminal"
			text: i18n("Terminal")
			enabled: currentServer.state === Objects.Server.State.Online
			onTriggered: {
				login_prompt.createObject(overlay, {source_page: page}).open();
			}
			tooltip: i18n("Opens a terminal")
		},
		Kirigami.Action {
			icon.name: "system-shutdown"
			text: currentServer.state === Objects.Server.State.Online ? i18n("Shutdown") : i18n("Power On")
			onTriggered: {
				if (currentServer.state === Objects.Server.State.Online) {
					currentServer.shutdown();
				}
				else {
					currentServer.wake_up();
				}
			}
			tooltip: currentServer.state === Objects.Server.State.Online ? i18n("Powers down the computer") : i18n("Powers up the computer")
		},
		Kirigami.Action {
			icon.name: "system-reboot"
			text: i18n("Reboot")
			enabled: currentServer.state === Objects.Server.State.Online
			onTriggered: {
				currentServer.reboot();
			}
			tooltip: "Reboots the computer"
		}
	]

	ColumnLayout {
		anchors.fill: parent
		
		Components.ServerInfo {}

		Kirigami.Separator {
			Layout.fillWidth: true
		}

		RowLayout {
			implicitWidth       : page.width
			implicitHeight      : 0
			Layout.margins      : 0
			Layout.maximumWidth : 0
			Layout.maximumHeight: 0
		}
	
		Controls.ScrollView {
			Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
			Controls.ScrollBar.vertical.policy  : Controls.ScrollBar.AsNeeded
			
			Layout.fillWidth : true
			Layout.fillHeight: true
	
			clip: true
	
			ColumnLayout {
				Layout.fillWidth : true
				Layout.fillHeight: true

				width: page.width

				Components.ColumnBlock {
					title: "Adapters"
					Components.Adapters {}
				}

				Components.ColumnBlock {
					title: "Drives"
					Components.Drives {}
				}
				
				Components.ColumnBlock {
					title: "Updates (" + currentServer.updates.count + ")"
					Components.UpdateList {}
				}
			}
		}
	}

	function open_term(term) {
		root.fullWindowLayer(terminalPage, {server: term});
	}

	Component {
		id: terminalPage
		Pages.Term {}
	}
	Component {
		id: login_prompt
		Dialogs.Login {}
	}
}
