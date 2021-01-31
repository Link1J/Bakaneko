// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.14
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls

Rectangle {
	property alias columnWidthProvider: table.columnWidthProvider
	property alias model              : table.model
	property alias delegate           : table.delegate

	readonly property alias contentWidth : table.contentWidth 
	readonly property alias contentHeight: table.contentHeight

	readonly property real  scrollWidth  : table.Controls.ScrollBar.vertical  .visible * (table.Controls.ScrollBar.vertical  .width )
	readonly property real  scrollHeight : table.Controls.ScrollBar.horizontal.visible * (table.Controls.ScrollBar.horizontal.height)
	readonly property real  totalWidth   : contentWidth  + table.leftMargin + scrollWidth  + 1
	readonly property real  totalHeight  : contentHeight + table.topMargin  + scrollHeight + 1

	color: "transparent" // Kirigami.Theme.backgroundColor

	function forceLayout() {
		table.forceLayout();
	}

	function defaultColumnWidthProvider(column) {
		var sum = 0;
		
		if (table.hasColumnHeaders) {
			sum = metrics.boundingRect(model.headerData(column, Qt.Horizontal)).width + Kirigami.Units.largeSpacing * 2 + 1;
		}

		for (var i = 0; i < model.rowCount(); i++) {
			var width = metrics.boundingRect(model.data(model.index(i, column), Qt.DisplayRole)).width + Kirigami.Units.largeSpacing * 2 + 1;
			if (width > sum) {
				sum = width;
			}
		}
		return sum;
	}
	
	function defaultRowHeightProvider(row) {
		var sum = 0;
		
		if (table.hasRowHeaders) {
			sum = metrics.boundingRect(model.headerData(row, Qt.Vertical)).height + Kirigami.Units.largeSpacing * 2 + 1;
		}
	
		for (var i = 0; i < model.columnCount(); i++) {
			var height = metrics.boundingRect(model.data(model.index(row, i), Qt.DisplayRole)).height + Kirigami.Units.largeSpacing * 2 + 1;
			if (height > sum) {
				sum = height;
			}
		}
		return sum;
	}

	TableView {
		id: table
		clip: true

		property bool hasColumnHeaders: table.model.headerData(-1, Qt.Horizontal) !== 0
		property bool hasRowHeaders   : table.model.headerData(-1, Qt.Vertical  ) !== 0

		anchors.fill: parent

		Controls.ScrollBar.horizontal: Controls.ScrollBar {}
		Controls.ScrollBar.vertical  : Controls.ScrollBar {}

		topMargin : hasColumnHeaders ? columnsHeader.implicitHeight : 0
		leftMargin: hasRowHeaders    ? rowsHeader   .implicitHeight : 0

		Component.onCompleted: {
			console.log(topMargin, leftMargin)
		}

		contentWidth: {
			var sum = 0;
			for (var i = 0; i < table.columns; i++) {
				sum += table.columnWidthProvider(i);
			}
			return sum;
		}

		contentHeight: {
			var sum = 0;
			for (var i = 0; i < table.rows; i++) {
				sum += table.rowHeightProvider(i);
			}
			return sum;
		}

		boundsBehavior: Flickable.StopAtBounds

		delegate: Item {
			text: display
		}

		columnWidthProvider: parent.defaultColumnWidthProvider
		rowHeightProvider  : parent.defaultRowHeightProvider
		
		property real remainHeight: table.parent.height - table.contentHeight - (table.topMargin  + 1)
		property real remainWidth : table.parent.width  - table.contentWidth  - (table.leftMargin + 1)

		Column {
			id: columnsHeader
			y: table.contentY
			z: 3
			Kirigami.Separator {
				width: table.contentWidth + table.leftMargin + (table.remainWidth > 0 ? table.remainWidth : 0)
				height: implicitHeight
			}
			Row {
				visible: table.hasColumnHeaders
				Repeater {
					model: table.columns >= 0 ? table.columns : 0
					Header {
						width: table.columnWidthProvider(modelData)
						text: table.model.headerData(modelData, Qt.Horizontal)

						Connections {
							target: table.model
							function onDataChanged() {
								parent.width = table.columnWidthProvider(modelData);
							}
						}
					}
				}
				Header {
					x: table.contentWidth + table.leftMargin
					width: table.remainWidth > 0 ? table.remainWidth : 0
					visible: table.remainWidth > 0
				}
			}
		}

		Row {
			id: rowsHeader
			x: table.contentX
			y: -table.topMargin
			z: 4
			Kirigami.Separator {
				width: implicitWidth
				height: table.contentHeight + table.topMargin + (table.remainHeight > 0 ? table.remainHeight : 0)
			}
			//Column {
			//	visible: table.hasRowsHeaders
			//	Repeater {
			//		model: table.rows >= 0 ? table.rows : 0
			//		Header {
			//			height: table.rowHeightProvider(modelData)
			//			text: table.model.headerData(modelData, Qt.Vertical)
			//
			//			Connections {
			//				target: table.model
			//				function onDataChanged() {
			//					parent.height = table.rowHeightProvider(modelData);
			//				}
			//			}
			//		}
			//	}
			//	Header {
			//		Layout.fillWidth: true
			//		y: table.contentHeight + table.leftMargin
			//		height: table.remainHeight > 0 ? table.remainHeight : 0
			//		visible: table.remainHeight > 0
			//	}
			//}
		}

		Row {
			id: columnsExtra
			y: table.contentY + table.contentHeight + table.topMargin
			z: 3
			Repeater {
				model: table.columns >= 0 ? table.columns : 0
				Item {
					width: table.columnWidthProvider(modelData)
					height: table.remainHeight > 0 ? table.remainHeight : 0
					visible: table.remainHeight > 0

					Connections {
						target: table.model
						function onDataChanged() {
							parent.width = table.columnWidthProvider(modelData);
						}
					}
				}
			}
			Item {
				x: table.contentY + table.contentHeight + table.topMargin
				width: table.remainWidth > 0 ? table.remainWidth : 0
				height: table.remainHeight > 0 ? table.remainHeight : 0
				visible: table.remainHeight > 0
			}
		}

		Connections {
			target: table.model
			function onDataChanged() {
				table.forceLayout();
			}
		}

		FontMetrics {
			id: metrics
			font: Kirigami.Theme.defaultFont
		}
	}

	component Item : ColumnLayout {
		property alias text: row.text
		property alias horizontalAlignment: row.horizontalAlignment

		spacing: 0

		RowLayout {
			property alias text: label.text
			property alias horizontalAlignment: label.horizontalAlignment

			Layout.fillWidth: true
			Layout.margins  : 0

			id: row

			spacing: 0

			Controls.Label {
				Layout.fillWidth: true
				Layout.margins  : 0

				id: label
				padding: Kirigami.Units.largeSpacing
				font   : Kirigami.Theme.defaultFont
				color  : Kirigami.Theme.textColor
			}
			Kirigami.Separator {
				Layout.margins  : 0
				Layout.fillHeight: true
			}
		}
		Kirigami.Separator {
			Layout.margins  : 0
			Layout.fillWidth: true
		}
	}

	component Header : ColumnLayout {
		Kirigami.Theme.colorSet: Kirigami.Theme.Header
		Kirigami.Theme.inherit: false

		property alias text: row.text
		property alias horizontalAlignment: row.horizontalAlignment

		spacing: 0

		RowLayout {
			property alias text: label.text
			property alias horizontalAlignment: label.horizontalAlignment

			Layout.fillWidth: true
			Layout.margins  : 0

			id: row

			spacing: 0

			Controls.Label {
				Layout.fillWidth: true
				Layout.margins  : 0

				background: Rectangle {
					color: Kirigami.Theme.backgroundColor
				}

				id: label
				padding: Kirigami.Units.largeSpacing
				font   : Kirigami.Theme.defaultFont
				color  : Kirigami.Theme.textColor
			}
			Kirigami.Separator {
				Layout.margins  : 0
				Layout.fillHeight: true
			}
		}
		Kirigami.Separator {
			Layout.margins  : 0
			Layout.fillWidth: true
		}
	}
}