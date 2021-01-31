// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Pages 1.0 as Pages
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components
import Bakaneko.Dialogs 1.0 as Dialogs
import Bakaneko.Managers 1.0 as Managers

Kirigami.ScrollablePage {
	title: i18n("Server List")

	Kirigami.Action {
		id: serverAddAction
		icon.name: "list-add"
		text: i18n("Add Server")
		visible: listView.count !== 0
		onTriggered: {
			addServerDialog.createObject(overlay).open();
		}
		tooltip: i18n("Adds server to list")
	}

	actions {
		main: serverAddAction
	}

	ListView {
		id: listView
		Kirigami.PlaceholderMessage {
			anchors.centerIn: parent
			width: parent.width - (Kirigami.Units.largeSpacing * 4)
			visible: listView.count === 0
			text: i18n("No Servers")
			helpfulAction: serverAddAction
		}
		delegate: Kirigami.SwipeListItem {
			highlighted: currentServer && currentServer.ip === ip
			focus: false
			Components.ServerListItem {}
			actions: [
				 Kirigami.Action {
					 icon.name: "settings-configure"
					 onTriggered: {
						serverSettingsDialog.createObject(overlay, {serverIndex: index, server: model.data}).open();
					 }
					 tooltip: i18n("Change server settings")
				 },
				 Kirigami.Action {
					 icon.name: "list-remove"
					 onTriggered: {
						 Managers.Server.RemoveServer(index)
					 }
					 tooltip: i18n("Removes server from list")
				 }
			]
		}
		model: Models.ServerList {
			onRowsRemoved: {
				if (currentServerIndex >= first && currentServerIndex <= last)
				{
					while (pageStack.depth > 1)
						pageStack.pop();
					currentServer = null
					currentServerIndex = -1;
				}
			}
		}
	}

	Component {
		id: addServerDialog
		Dialogs.AddServer {}
	}
	Component {
		id: serverSettingsDialog
		Dialogs.ServerSettings {}
	}
}