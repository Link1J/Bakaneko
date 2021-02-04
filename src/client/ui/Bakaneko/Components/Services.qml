// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.14
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.8 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components

Kirigami.Page {
	Kirigami.Theme.colorSet: Kirigami.Theme.View

	id: addServerDialog
	title: currentServer.serviceManager
	padding: 0
	
	header: 
	Controls.ToolBar {
		RowLayout {
			anchors.fill: parent
			Controls.ComboBox {
				Layout.maximumWidth: 100 * Kirigami.Units.devicePixelRatio
				model: currentServer.serviceTypes
				textRole: "display"
			}
			Kirigami.SearchField {
				Layout.fillWidth: true
			}
		}
	}

	Components.Table {
		id: table
		anchors.fill: parent

		columnWidthProvider: function (column) {
			if (column === 2) {
				return dynamic_column(column);
			}
			return defaultColumnWidthProvider(column);
		}

		model: currentServer.services
	}
}