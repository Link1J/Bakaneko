// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.6
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components

Kirigami.ScrollablePage {
	id: addServerDialog
	title: "Drives"

	ListView {
		id: listView
		model: currentServer.drives
		Kirigami.PlaceholderMessage {
			anchors.centerIn: parent
			width: parent.width - (Kirigami.Units.largeSpacing * 4)
			visible: listView.count === 0
			text: "なに？" // Do not translate.
		}
		delegate: Kirigami.AbstractListItem {
			focus: false

			onFocusChanged: {
				if (focus !== false)
					focus = false
			}

			action: Kirigami.Action {
				onTriggered: {
					data.info.visible = !data.info.visible
				}
			}

			ColumnLayout {
				id: data

				RowLayout {
					Kirigami.Heading {
						Layout.fillWidth: true
						text: dev_node
					}
					Kirigami.Icon {
						source: info.visible ? "draw-arrow-up" : "draw-arrow-down"
					}
				}

				property var info: info
				ColumnLayout {
					id: info
					visible: false

					Grid {
						Layout.fillWidth: true
						spacing: Kirigami.Units.largeSpacing

						Controls.Label {
							visible: model != ""
							text: "Model: " + model
						}
						Controls.Label {
							text: "Size: " + size
						}
					}

					Components.Table {
						Layout.fillWidth: true

						Component.onCompleted: {
							forceLayout();
						}

						model: partitions
						implicitHeight: totalHeight
						visible: partitions.count >= 0
						columnWidthProvider: function (column) {
							if (column === 3) {
								return metrics.boundingRect("10000 TB").width + Kirigami.Units.largeSpacing * 2;
							}
							if (column === 4) {
								return metrics.boundingRect("1000%").width + Kirigami.Units.largeSpacing * 2;
							}
							if (column === 2) {
								return dynamic_column(column);
							}
							return defaultColumnWidthProvider(column);
						}
						FontMetrics {
							id: metrics
							font: Kirigami.Theme.defaultFont
						}
						delegate: DelegateChooser {
							DelegateChoice {
								column: 0
								delegate: Components.Table.Item {
									text: model.dev_node
									leftBorder: true
								}
							}
							DelegateChoice {
								column: 1
								delegate: Components.Table.Item {
									text: model.filesystem
								}
							}
							DelegateChoice {
								column: 2
								delegate: Components.Table.Item {
									text: model.mountpoint
								}
							}
							DelegateChoice {
								column: 3
								delegate: Components.Table.Item {
									horizontalAlignment: Text.AlignRight
									text: model.size
								}
							}
							DelegateChoice {
								column: 4
								delegate: Components.Table.Item {
									horizontalAlignment: Text.AlignRight
									text: model.used
								}
							}
						}
					}
				}
			}
		}
	}
}