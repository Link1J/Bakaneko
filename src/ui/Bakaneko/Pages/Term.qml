// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.1
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Objects 1.0 as Objects
import Bakaneko.Managers 1.0 as Managers
import Bakaneko.Components 1.0 as Components

Kirigami.ScrollablePage {
	title: i18n("Terminal")
	Components.Screen {
		id: screen
		server: currentServer
		focus: true
	}
}