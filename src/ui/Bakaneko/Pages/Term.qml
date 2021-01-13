// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.1
import org.kde.kirigami 2.5 as Kirigami
import Bakaneko.Components 1.0 as Components

Kirigami.ScrollablePage {
	title: i18n("Terminal")
	padding: 5
	Components.Screen {
		id: screen
		server: currentServer
		focus: true
	}
}