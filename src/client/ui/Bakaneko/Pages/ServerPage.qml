// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.12
import QtQuick.Layouts 1.3
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

	header:
	Controls.ToolBar {
		height: bar.contentHeight + Kirigami.Units.smallSpacing
		Controls.TabBar {
			id: bar
			width: parent.width
			Controls.TabButton {
				display: Controls.AbstractButton.TextBesideIcon
				text: i18n("Info")
				icon.name: ""
			}
			Controls.TabButton {
				display: Controls.AbstractButton.TextBesideIcon
				text: i18n("Network")
				icon.name: "network-wired"
				enabled: currentServer.avaliable_adapters
			}
			Controls.TabButton {
				display: Controls.AbstractButton.TextBesideIcon
				text: i18n("Drives")
				icon.name: "drive-harddisk"
				enabled: currentServer.avaliable_drives
			}
			Controls.TabButton {
				display: Controls.AbstractButton.TextBesideIcon
				text: i18n("Updates")
				icon.name: info_colunm.is_breeze_theme() ? "update-none" : "system-software-update"
				enabled: currentServer.avaliable_updates
			}
			Controls.TabButton {
				display: Controls.AbstractButton.TextBesideIcon
				text: i18n("Services")
				icon.name: info_colunm.is_breeze_theme() ? "system-run" : "preferences-system-services"
				enabled: currentServer.avaliable_services
			}
			Connections {
				target: currentServer
				function onChanged_enabled_pages() {
					if (!bar.data[bar.currentIndex].enabled)
						bar.currentIndex = 0;
				}
			}
		}
	}

	ColumnLayout {
		id: info_colunm
		anchors.fill: parent

		function is_breeze_theme() {
			return Managers.AppInfo.icon_theme === "breeze" || Managers.AppInfo.icon_theme === "breeze-dark" || Managers.AppInfo.icon_theme === "kf5_rcc_theme";
		}

		StackLayout {
			width: parent.width
			currentIndex: bar.currentIndex

			Components.ServerInfo {}
			Components.Adapters {
				Layout.fillWidth: true
				Layout.fillHeight: true
			}
			Components.Drives {}
			Components.UpdateList {
				Layout.fillWidth: true
				Layout.fillHeight: true
			}
			Components.Services {}
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
