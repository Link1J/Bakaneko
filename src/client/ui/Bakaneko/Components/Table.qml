// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.14
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.0 as Controls

Rectangle {
	id: base

	property alias columnWidthProvider: table.columnWidthProvider
	property alias rowHeightProvider  : table.rowHeightProvider
	property alias model              : table.model
	property alias delegate           : table.delegate

	property bool showHeaders: true

	readonly property alias contentWidth : table.contentWidth
	readonly property alias contentHeight: table.contentHeight
	readonly property alias columns      : table.columns
	readonly property alias rows         : table.rows
	readonly property alias contentY     : table.contentY
	readonly property alias contentX     : table.contentX

	readonly property real  scrollWidth  : table.Controls.ScrollBar.vertical  .visible * (table.Controls.ScrollBar.vertical  .width )
	readonly property real  scrollHeight : table.Controls.ScrollBar.horizontal.visible * (table.Controls.ScrollBar.horizontal.height)
	readonly property real  totalWidth   : contentWidth  + table.leftMargin + scrollWidth
	readonly property real  totalHeight  : contentHeight + table.topMargin  + scrollHeight

	readonly property var tableView : table

	color: "transparent" // Kirigami.Theme.backgroundColor

	function forceLayout() {
		table.forceLayout();
	}

	onWidthChanged: {
		forceLayout();
	}

	function dynamic_column(column) {
		var mins = defaultColumnWidthProvider(column);
		var sum = width - scrollWidth;
		for (var a = 0; a < columns; a++) {
			if (a === column) continue;
			sum -= columnWidthProvider(a);
		}
		if (sum >= mins) {
			return sum;
		}
		return mins;
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

		topMargin : (hasColumnHeaders && base.showHeaders) ? columnsHeader.implicitHeight : 0
		leftMargin: (hasRowHeaders                       ) ? rowsHeader   .implicitHeight : 0

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
			leftBorder: !(base.anchors.fill === base.parent && column === 0)
			rightBorder: !(base.anchors.fill === base.parent && column === table.columns - 1 && table.remainWidth <= 0)
		}

		columnWidthProvider: parent.defaultColumnWidthProvider
		rowHeightProvider  : parent.defaultRowHeightProvider
		
		property real remainHeight: table.parent.height - table.contentHeight - (table.topMargin )
		property real remainWidth : table.parent.width  - table.contentWidth  - (table.leftMargin)

		Column {
			id: columnsHeader
			y: table.contentY
			z: 3
			visible: base.showHeaders
			Rectangle {
				Kirigami.Theme.colorSet: Kirigami.Theme.Header
				Kirigami.Theme.inherit: false
				color: Kirigami.Theme.backgroundColor
				width : table.contentWidth + table.leftMargin + (table.remainWidth > 0 ? table.remainWidth : 0)
				height: header.height
				visible: base.showHeaders
				
				onWidthChanged: {
					header.updateSize();
				}

				Header {
					id: header
					table: table
					_base: base
				}
			}
		}

		Row {
			id: columnsExtra
			y: table.contentY + table.contentHeight + table.topMargin
			z: 3

			width : table.contentWidth + table.leftMargin + (table.remainWidth > 0 ? table.remainWidth : 0)
			
			onWidthChanged: {
				updateSize();
			}

			signal updateSize()

			Repeater {
				model: table.columns >= 0 ? table.columns : 0
				Item {
					id: headerItem

					width: table.columnWidthProvider(modelData)
					height: table.remainHeight > 0 ? table.remainHeight : 0
					text : ""
					leftBorder  : base.anchors.fill !== base.parent ? modelData === 0 : false
					bottomBorder: base.anchors.fill !== base.parent
					rightBorder : !(base.anchors.fill === base.parent && modelData === table.columns - 1 && table.remainWidth <= base.scrollWidth)

					Connections {
						target: table.model
						function onDataChanged() {
							headerItem.width = table.columnWidthProvider(modelData);
						}
					}
					Connections {
						target: columnsExtra
						function onUpdateSize() {
							headerItem.width = table.columnWidthProvider(modelData);
						}
					}
				}
			}
			Item {
				x: table.contentWidth + table.leftMargin
				width: table.remainWidth > 0 ? table.remainWidth : 0
				visible: table.remainWidth > 0

				bottomBorder: base.anchors.fill !== base.parent
				rightBorder : base.anchors.fill !== base.parent
			}
		}

			/*Repeater {
				model: table.columns >= 0 ? table.columns : 0
				Item {
					width: table.columnWidthProvider(modelData)
					visible: table.remainHeight > 0
					bottomBorder: false
					rightBorder: !(base.anchors.fill === base.parent && modelData === table.columns - 1 && table.remainWidth <= base.scrollWidth)

					Connections {
						target: table.model
						function onDataChanged() {
							parent.width = table.columnWidthProvider(modelData);
						}
					}
				}
			}*/

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
		
		property bool topBorder   : false
		property bool leftBorder  : false
		property bool bottomBorder: true
		property bool rightBorder : true

		spacing: 0

		Kirigami.Separator {
			Layout.margins  : 0
			Layout.fillWidth: true
			visible: topBorder
		}
		RowLayout {
			property alias text: label.text
			property alias horizontalAlignment: label.horizontalAlignment

			Layout.fillWidth: true
			Layout.margins  : 0

			id: row

			spacing: 0

			Kirigami.Separator {
				Layout.margins  : 0
				Layout.fillHeight: true
				visible: leftBorder
			}
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
				visible: rightBorder
			}
		}
		Kirigami.Separator {
			Layout.margins  : 0
			Layout.fillWidth: true
			visible: bottomBorder
		}
	}

	component Header : Row {
		id: row

		property var table
		property var _base: null

		Component.onCompleted: {
			if (table.tableView !== undefined) {
				_base = table;
				table = _base.tableView;
			}
		}
		
		onWidthChanged: {
			updateSize();
		}

		signal updateSize()

		Repeater {
			model: table.columns >= 0 ? table.columns : 0
			Item {
				id: headerItem

				width: table.columnWidthProvider(modelData)
				text : table.model.headerData(modelData, Qt.Horizontal)
				topBorder   : _base.showHeaders && _base.anchors.fill !== _base.parent
				leftBorder  : _base.anchors.fill !== _base.parent ? modelData === 0 : false
				bottomBorder: _base.showHeaders
				rightBorder : !(_base.anchors.fill === _base.parent && modelData === table.columns - 1 && table.remainWidth <= _base.scrollWidth)

				Connections {
					target: table.model
					function onDataChanged() {
						headerItem.width = table.columnWidthProvider(modelData);
					}
				}
				Connections {
					target: row
					function onUpdateSize() {
						headerItem.width = table.columnWidthProvider(modelData);
					}
				}
			}
		}
		Item {
			x: table.contentWidth + table.leftMargin
			width: table.remainWidth > 0 ? table.remainWidth : 0
			visible: table.remainWidth > 0 && _base.showHeaders

			topBorder   : _base.showHeaders && _base.anchors.fill !== _base.parent
			bottomBorder: _base.showHeaders
			rightBorder : _base.anchors.fill !== _base.parent
		}
	}

	component HeaderBar : Flickable {
		id: scroll
		property alias table: header.table

		interactive: false
		contentX: header.table.contentX
		contentHeight: header.height
		height: header.height
		
		onWidthChanged: {
			header.updateSize();
		}

		Header {
			id: header
		}
	}
}