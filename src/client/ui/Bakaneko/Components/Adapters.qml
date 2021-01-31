// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.14
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components

Kirigami.Page {
	Kirigami.Theme.colorSet: Kirigami.Theme.View

	id: addServerDialog
	title: "Network Adapters"
	padding: 0

	Components.Table {
		columnWidthProvider: function (column) {
			if (column >= 2 && column <= 4) {
				return metrics.boundingRect("10000000 Tbps").width + Kirigami.Units.largeSpacing * 2;
			}
			return defaultColumnWidthProvider(column);
		}

		FontMetrics {
			id: metrics
			font: Kirigami.Theme.defaultFont
		}

		model: currentServer.adapters
		anchors.fill: parent

		delegate: DelegateChooser {
			DelegateChoice {
				column: 0
				delegate: Components.Table.Item {
					text: model.name
				}
			}
			DelegateChoice {
				column: 1
				delegate: Components.Table.Item {
					text: model.connection_state
				}
			}
			DelegateChoice {
				column: 2
				delegate: Components.Table.Item {
					horizontalAlignment: (model.link_speed != "Unknown")
						? Text.AlignRight
						: Text.AlignLeft
					text: model.link_speed
				}
			}
			DelegateChoice {
				column: 3
				delegate: Components.Table.Item {
					horizontalAlignment: Text.AlignRight
					text: model.tx_rate
				}
			}
			DelegateChoice {
				column: 4
				delegate: Components.Table.Item {
					horizontalAlignment: Text.AlignRight
					text: model.rx_rate
				}
			}
		}
	}
}