// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.4
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models

Grid {
	id: base
	columns: 3
	columnSpacing: Kirigami.Units.gridUnit

	Repeater {
		model: currentServer.updates
		Repeater {
			model: [name, old_version, new_version]

			Controls.Label {
				text: modelData
			}
		}
	}
}
