// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.12
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.9 as Controls
import Bakaneko.Objects 1.0 as Objects
import Bakaneko.Pages 1.0 as Pages
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components
import Bakaneko.Dialogs 1.0 as Dialogs
import Bakaneko.Managers 1.0 as Managers

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
		id: info_colunm
		anchors.fill: parent
		spacing: 0
		
		Components.ServerInfo {}

		Kirigami.Separator {
			Layout.fillWidth   : true
			Layout.bottomMargin: 0
			Layout.leftMargin  : 0
			Layout.rightMargin : 0
			Layout.topMargin   : 5
		}

		RowLayout {
			implicitWidth       : page.width
			implicitHeight      : 0
			Layout.margins      : 0
			Layout.maximumWidth : 0
			Layout.maximumHeight: 0
		}

		ListView {
			Layout.fillWidth : true
			Layout.fillHeight: true

			clip        : true
			bottomMargin: 0
			leftMargin  : 0
			rightMargin : 0
			topMargin   : 0

			model: [
				{
					name: "Network",
					icon: "network-wired",
					page: adapters_page
				},
				{
					name: "Drives",
					icon: "drive-harddisk",
					page: drives_page
				},
				{
					name: "Updates",
					icon: info_colunm.is_breeze_theme() ? "update-none" : "system-software-update",
					page: updates_page
				},
				{
					name: "Services",
					icon: info_colunm.is_breeze_theme() ? "system-run" : "preferences-system-services",
					page: services_page
				}
			]
			delegate: Kirigami.BasicListItem {
				highlighted: (pageStack.get(2) ? pageStack.get(2).objectName === modelData.name : false)
				label: modelData.name
				icon: {
					name: modelData.icon
				}
				TapHandler {
					onTapped: {
						if (pageStack.depth === 3) {
							pageStack.currentIndex = 2;
							pageStack.replace(modelData.page);
						}
						else {
							pageStack.push(modelData.page);
						}
					}
				}
			}
		}

		function is_breeze_theme() {
			return Managers.AppInfo.icon_theme === "breeze" || Managers.AppInfo.icon_theme === "breeze-dark" || Managers.AppInfo.icon_theme === "kf5_rcc_theme";
		}
		Component {
			id: adapters_page
			Components.Adapters {
				objectName: "Network"
			}
		}
		Component {
			id: drives_page
			Components.Drives {
				objectName: "Drives"
			}
		}
		Component {
			id: updates_page
			Components.UpdateList {
				objectName: "Updates"
			}
		}
		Component {
			id: services_page
			Components.Services {
				objectName: "Services"
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
		Dialogs.Login {
			onRun: function (logindata) {
				currentServer.open_term(logindata);
			}
		}
	}
}
