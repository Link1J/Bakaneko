// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.4
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls

ColumnLayout {
	Layout.fillWidth : true

	        property alias title   : group.title
	default property alias contents: place.data

	Controls.GroupBox {
		Layout.fillWidth : true
		Layout.margins: Kirigami.Units.smallSpacing
		
		id: group

		background: Rectangle {
			color: "transparent"
			border.color: "transparent"
			radius: 2
		}

		Item {
			id: place
			implicitHeight: {
				var temp_height = 0
				for (var i = 0; i < data.length; i++) {
					temp_height += data[i].implicitHeight
				}
				return temp_height
			}
		}
	}

	Kirigami.Separator {
		id: sep
		Layout.fillWidth: true
	}
}
