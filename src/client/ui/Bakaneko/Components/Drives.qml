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
				implicitWidth: parent.implicitWidth

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
						onWidthChanged: {
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
								var mins = defaultColumnWidthProvider(column);
								var sum = width - (columnWidthProvider(0) + columnWidthProvider(1) + columnWidthProvider(3) + columnWidthProvider(4));
								if (sum >= mins) {
									return sum;
								}
								return mins;
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

	/*Column {
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
	
				title: dev_node
				leftPadding: Kirigami.Units.largeSpacing
	
				ColumnLayout {
					Grid {
						Layout.fillWidth: true
						leftPadding: Kirigami.Units.largeSpacing
						spacing: Kirigami.Units.largeSpacing
	
						Controls.Label {
							visible: model != ""
							text: "Model: " + model
						}
						Controls.Label {
							text: "Size: " + size
						}
					}
					Controls.Label {
						Layout.fillWidth: true
						visible: partitions.count !== 0
						topPadding: Kirigami.Units.smallSpacing
						leftPadding: Kirigami.Units.largeSpacing
						text: "Partitions: "
					}
					Components.Table {
						Layout.fillWidth: true
						model: partitions
						implicitHeight: contentHeight + Kirigami.Units.largeSpacing
						visible: partitions.count !== 0
						delegate: DelegateChooser {
							DelegateChoice {
								column: 0
								delegate: Components.Table.Item {
									text: model.dev_node
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

					Row {
						leftPadding: Kirigami.Units.largeSpacing * 2
						GridLayout {
							columnSpacing: Kirigami.Units.largeSpacing
							rowSpacing: 0
	
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 0
								text: "Device Node"
							}
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 1
								text: "Filesystem"
							}
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 2
								text: "Mountpoint"
							}
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 3
								text: "Size"
							}
							Controls.Label {
								Layout.fillWidth: true
								Layout.column: 4
								text: "Used"
							}
	
							Kirigami.Separator {
								Layout.row: 1
								Layout.column: 0
								Layout.columnSpan: 9
								Layout.fillWidth: true
							}
	
							Repeater {
								model: partitions
								Controls.Label {
									Layout.fillWidth: true
									Layout.column: 0
									Layout.row: index + 2
									text: dev_node
								}
							}
							Repeater {
								model: partitions
								Controls.Label {
									Layout.fillWidth: true
									Layout.column: 1
									Layout.row: index + 2
									text: filesystem
								}
							}
							Repeater {
								model: partitions
								Controls.Label {
									Layout.fillWidth: true
									Layout.column: 2
									Layout.row: index + 2
									text: mountpoint
								}
							}
							Repeater {
								model: partitions
								Controls.Label {
									Layout.fillWidth: true
									Layout.column: 3
									Layout.row: index + 2
									horizontalAlignment: Text.AlignRight
									text: size
								}
							}
							Repeater {
								model: partitions
								Controls.Label {
									Layout.fillWidth: true
									Layout.column: 4
									Layout.row: index + 2
									horizontalAlignment: Text.AlignRight
									text: used
								}
							}
						}
					}
				}
			}
		}
	}*/
}