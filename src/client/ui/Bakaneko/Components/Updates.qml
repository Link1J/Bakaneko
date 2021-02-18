// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.4
import Qt.labs.qmlmodels 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components

Components.Table {
	id: table

	model: currentServer.updates
	
	columnWidthProvider: function (column) {
		if (column === 0) {
			return dynamic_column(column);
		}
		var mins = defaultColumnWidthProvider(column);
		var sum = metrics.boundingRect("WWWWWWWWWW").width + Kirigami.Units.largeSpacing * 2;
		if (sum >= mins) {
			return sum;
		}
		return mins;
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
				rightBorder: false
			}
		}
	}
}
