// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.4
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models

GridLayout {
	columnSpacing: Kirigami.Units.largeSpacing
	rowSpacing: 0

	Controls.Label {
		Layout.fillWidth: true
		Layout.column: 0
		text: "Name"
	}
	Kirigami.Separator {
		Layout.row: 0
		Layout.column: 1
		Layout.rowSpan: currentServer.adapters.count + 2
		Layout.fillHeight: true
	}
	Controls.Label {
		Layout.fillWidth: true
		Layout.column: 2
		text: "Link Speed"
	}
	Kirigami.Separator {
		Layout.row: 0
		Layout.column: 3
		Layout.rowSpan: currentServer.adapters.count + 2
		Layout.fillHeight: true
	}
	Controls.Label {
		Layout.fillWidth: true
		Layout.column: 4
		text: "State"
	}
	Kirigami.Separator {
		Layout.row: 0
		Layout.column: 5
		Layout.rowSpan: currentServer.adapters.count + 2
		Layout.fillHeight: true
	}
	Controls.Label {
		Layout.fillWidth: true
		Layout.column: 6
		text: "Send"
	}
	Kirigami.Separator {
		Layout.row: 0
		Layout.column: 7
		Layout.rowSpan: currentServer.adapters.count + 2
		Layout.fillHeight: true
	}
	Controls.Label {
		Layout.fillWidth: true
		Layout.column: 8
		text: "Received"
	}
	Kirigami.Separator {
		Layout.row: 1
		Layout.column: 0
		Layout.columnSpan: 9
		Layout.fillWidth: true
	}

	Repeater {
		model: currentServer.adapters
		Controls.Label {
			Layout.fillWidth: true
			Layout.column: 0
			Layout.row: index + 2
			text: name
		}
	}
	Repeater {
		model: currentServer.adapters
		Controls.Label {
			Layout.fillWidth: true
			Layout.column: 2
			Layout.row: index + 2
			horizontalAlignment: (link_speed != "Unknown") ? Text.AlignRight : Text.AlignLeft
			text: link_speed
		}
	}
	Repeater {
		model: currentServer.adapters
		Controls.Label {
			Layout.fillWidth: true
			Layout.column: 4
			Layout.row: index + 2
			text: connection_state
		}
	}
	Repeater {
		model: currentServer.adapters
		Controls.Label {
			Layout.fillWidth: true
			Layout.column: 6
			Layout.row: index + 2
			horizontalAlignment: Text.AlignRight
			text: tx_rate
		}
	}
	Repeater {
		model: currentServer.adapters
		Controls.Label {
			Layout.fillWidth: true
			Layout.column: 8
			Layout.row: index + 2
			horizontalAlignment: Text.AlignRight
			text: rx_rate
		}
	}
}
