// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.4
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components

Kirigami.Page {
	Kirigami.Theme.colorSet: Kirigami.Theme.View

	id: addServerDialog
	title: "Updates"
	padding: 0

	Components.Table {
		model: currentServer.updates
		anchors.fill: parent
		
		columnWidthProvider: function (column) {
			if (column === 0) {
				var mins = defaultColumnWidthProvider(column);
				var sum = width - (columnWidthProvider(1) + columnWidthProvider(2));
				if (sum >= mins) {
					return sum;
				}
				return mins;
			}
			return metrics.boundingRect("WWWWWWWWWW").width + Kirigami.Units.largeSpacing * 2;
		}

		FontMetrics {
			id: metrics
			font: Kirigami.Theme.defaultFont
		}

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
					text: model.old_version
				}
			}
			DelegateChoice {
				column: 2
				delegate: Components.Table.Item {
					text: model.new_version
				}
			}
		}
	}
}
