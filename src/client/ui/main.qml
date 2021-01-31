// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.1
import org.kde.kirigami 2.6 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Pages 1.0 as Pages
import Bakaneko.Managers 1.0 as Managers

Kirigami.ApplicationWindow {
	id: root

	property var currentServer: null
	property int currentServerIndex: -1

	title: i18n("Server Manager")

	function fullWindowLayer(page, args) {
		if (pageStack.layers.depth === 2) {
			pageStack.layers.replace(page, args);
		} else {
			pageStack.layers.push(page, args);
		}
	}

	contextDrawer: Kirigami.ContextDrawer {
		id: contextDrawer
	}

	pageStack.initialPage: Pages.ServerList {}
	pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton
	pageStack.onPageInserted: {
		if (pageStack.currentIndex < position) {
			pageStack.currentIndex = position;
		}
	}

	globalDrawer: Kirigami.GlobalDrawer {
		actions: [
			Kirigami.Action {
				text: i18n("Settings")
				icon.name: "settings-configure"
				onTriggered: fullWindowLayer(settingsPage)
				enabled: pageStack.layers.currentItem.title !== i18n("Settings")
			},
			Kirigami.Action {
				text: i18n("About")
				icon.name: "help-about"
				onTriggered: fullWindowLayer(aboutPage)
				enabled: pageStack.layers.currentItem.title !== i18n("About")
			}
		]
	}

	Component {
		id: aboutPage
		Kirigami.AboutPage {
			aboutData: Managers.AppInfo.about
		}
	}
	Component {
		id: settingsPage
		Pages.Settings {
		}
	}
}
