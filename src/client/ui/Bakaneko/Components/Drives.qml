// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.6
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components

Column {
	id: base
	spacing: Kirigami.Units.largeSpacing

	Repeater {
		model: currentServer.drives
		Controls.GroupBox {
			id: group

			background: Kirigami.Separator {
				width : Math.floor(Kirigami.Units.devicePixelRatio)
				height: group.height
			}

			title: modelData.dev_node
			leftPadding: Kirigami.Units.largeSpacing

			Column {
				Grid {
					leftPadding: Kirigami.Units.largeSpacing
					spacing: Kirigami.Units.largeSpacing

					Controls.Label {
						visible: modelData.model != ""
						text: "Model: " + modelData.model
					}
					Controls.Label {
						text: "Size: " + modelData.size
					}
				}
				Controls.Label {
					visible: modelData.paritions.length != 0
					topPadding: Kirigami.Units.smallSpacing
					leftPadding: Kirigami.Units.largeSpacing
					text: "Partitions: "
				}
				Row {
					leftPadding: Kirigami.Units.largeSpacing * 2
					GridLayout {
						visible: modelData.paritions.length != 0
						columnSpacing: Kirigami.Units.largeSpacing
						rowSpacing: 0

						Controls.Label {
							Layout.fillWidth: true
							Layout.column: 0
							text: "Device Node"
						}
						Kirigami.Separator {
							Layout.row: 0
							Layout.column: 1
							Layout.rowSpan: modelData.paritions.length * 2 + 2
							Layout.fillHeight: true
						}
						Controls.Label {
							Layout.fillWidth: true
							Layout.column: 2
							text: "Filesystem"
						}
						Kirigami.Separator {
							Layout.row: 0
							Layout.column: 3
							Layout.rowSpan: modelData.paritions.length * 2 + 2
							Layout.fillHeight: true
						}
						Controls.Label {
							Layout.fillWidth: true
							Layout.column: 4
							text: "Mountpoint"
						}
						Kirigami.Separator {
							Layout.row: 0
							Layout.column: 5
							Layout.rowSpan: modelData.paritions.length * 2 + 2
							Layout.fillHeight: true
						}
						Controls.Label {
							Layout.fillWidth: true
							Layout.column: 6
							text: "Size"
						}
						Kirigami.Separator {
							Layout.row: 0
							Layout.column: 7
							Layout.rowSpan: modelData.paritions.length * 2 + 2
							Layout.fillHeight: true
						}
						Controls.Label {
							Layout.fillWidth: true
							Layout.column: 8
							text: "Used"
						}

						
						Repeater {
							model: modelData.paritions.length
							Kirigami.Separator {
								Layout.row: index * 2 + 1
								Layout.column: 0
								Layout.columnSpan: 9
								Layout.fillWidth: true
							}
						}
						Repeater {
							model: modelData.paritions
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 0
								Layout.row: index * 2 + 2
								text: modelData.dev_node
							}
						}
						Repeater {
							model: modelData.paritions
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 2
								Layout.row: index * 2 + 2
								text: modelData.filesystem
							}
						}
						Repeater {
							model: modelData.paritions
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 4
								Layout.row: index * 2 + 2
								text: modelData.mountpoint
							}
						}
						Repeater {
							model: modelData.paritions
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 6
								Layout.row: index * 2 + 2
								horizontalAlignment: Text.AlignRight
								text: modelData.size
							}
						}
						Repeater {
							model: modelData.paritions
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 8
								Layout.row: index * 2 + 2
								horizontalAlignment: Text.AlignRight
								text: modelData.used
							}
						}
					}
				}
			}
		}
	}
}
