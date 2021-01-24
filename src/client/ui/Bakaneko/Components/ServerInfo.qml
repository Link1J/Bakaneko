// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.4
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.9 as Controls
import Bakaneko.Objects 1.0 as Objects

RowLayout {
	Layout.alignment    : Qt.AlignTop | Qt.AlignLeft
	Layout.fillWidth    : false
	Layout.minimumHeight: icon.max_height
	Layout.maximumHeight: icon.max_height

	Kirigami.Icon {
		readonly property var max_height: 5 * (temp.height + Kirigami.Units.smallSpacing) + Kirigami.Units.smallSpacing - 1
		readonly property var icon_width: icon.width + Kirigami.Units.smallSpacing * 2 + icon.x
		readonly property var info_width: info.width + Kirigami.Units.largeSpacing * 4
		readonly property var page_width: page.width - page.leftPadding - page.rightPadding * 2

		id: icon

		Layout.alignment  : Qt.AlignLeft
		Layout.rightMargin: Kirigami.Units.smallSpacing
		Layout.leftMargin : Kirigami.Units.smallSpacing

		source        : currentServer.icon
		implicitHeight: implicitWidth
		implicitWidth : 
			page_width - info_width > 0
			? page_width - info.x < info_width
				? page_width - info_width < max_height
					? page_width - info_width
					: max_height
				: max_height
			: 0
	}

	Kirigami.FormLayout {
		id: info

		Layout.fillWidth: true
		Layout.fillHeight: true
		Layout.alignment: Qt.AlignTop | Qt.AlignLeft
		Layout.rightMargin: Kirigami.Units.smallSpacing

		wideMode: true

		Controls.Label {
			id: temp
			Kirigami.FormData.label: "OS:"
			text: currentServer.os
		}
		Controls.Label {
			Kirigami.FormData.label: "Kernel:"
			text: currentServer.kernel
		}
		Controls.Label {
			Kirigami.FormData.label: "Architecture:"
			text: currentServer.arch
		}
		Controls.Label {
			Kirigami.FormData.label: "IP:"
			text: currentServer.ip
		}
		Controls.Label {
			Kirigami.FormData.label: "MAC:"
			text: currentServer.mac
		}
	}
}