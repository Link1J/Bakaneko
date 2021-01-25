// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.1
import org.kde.kirigami 2.5 as Kirigami
import Bakaneko.Components 1.0 as Components

Kirigami.ScrollablePage {
	id: page

	title: i18n("Terminal")

	topPadding   : 0
	leftPadding  : 0
	rightPadding : 0
	bottomPadding: 0

	property var server

	//flickableItem.ScrollBar.vertical.width

	verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
	//Component.onCompleted: verticalScrollBarPolicy = Qt.ScrollBarAlwaysOn

	Components.Screen {
		id: screen
		focus: true
		Component.onCompleted: screen.server = page.server
	}
}