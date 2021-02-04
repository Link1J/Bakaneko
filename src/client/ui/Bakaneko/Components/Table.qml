// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.14
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.5 as Kirigami
import QtQuick.Controls 2.9 as Controls

Rectangle {
	id: base

	property alias columnWidthProvider: table.columnWidthProvider
	property alias rowHeightProvider  : table.rowHeightProvider
	property alias model              : table.model
	property alias delegate           : table.delegate
	property alias reuseItems         : table.reuseItems

	property bool showHeaders: true

	readonly property alias contentWidth : table.contentWidth
	readonly property alias contentHeight: table.contentHeight
	readonly property alias columns      : table.columns
	readonly property alias rows         : table.rows
	readonly property alias contentY     : table.contentY
	readonly property alias contentX     : table.contentX

	readonly property real  scrollWidth  : scrollView.Controls.ScrollBar.vertical  .visible * (scrollView.Controls.ScrollBar.vertical  .width )
	readonly property real  scrollHeight : scrollView.Controls.ScrollBar.horizontal.visible * (scrollView.Controls.ScrollBar.horizontal.height)
	readonly property real  totalWidth   : contentWidth  + scrollWidth  + 0
	readonly property real  totalHeight  : contentHeight + scrollHeight + columnsHeader.height

	readonly property bool fillsParent: anchors.fill === parent

	readonly property alias table : grid.table

	color: "transparent"

	function forceLayout() {
		table.forceLayout();
	}

	onWidthChanged: {
		forceLayout();
	}

	onScrollWidthChanged: {
		forceLayout();
	}

	onScrollHeightChanged: {
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
		if (model.columnMinWidth !== undefined) {
			return model.columnMinWidth(column);
		}
		else {
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
	}
	
	function defaultRowHeightProvider(row) {
		var sum = 0;
		
		if (table.hasRowHeaders) {
			sum = metrics.boundingRect(model.headerData(row, Qt.Vertical)).height + Kirigami.Units.largeSpacing * 2;
		}
	
		for (var i = 0; i < model.columnCount(); i++) {
			var height = metrics.boundingRect(model.data(model.index(row, i), Qt.DisplayRole)).height + Kirigami.Units.largeSpacing * 2;
			if (height > sum) {
				sum = height;
			}
		}
		return sum;
	}

	GridLayout {
		id: grid
		columns: 2
		rows   : 2

		columnSpacing: 0
		rowSpacing   : 0

		anchors.fill: parent

		property alias table: scrollView.table
		property var scrollView: scrollView
		property var columnsHeader: columnsHeader

		Rectangle {
			Layout.row   : 0
			Layout.column: 1
			Layout.fillWidth : true

			id: columnsHeader
			visible: base.showHeaders

			Kirigami.Theme.colorSet: Kirigami.Theme.Header
			Kirigami.Theme.inherit: false
			color: Kirigami.Theme.backgroundColor
			implicitHeight: header.height

			onWidthChanged: {
				header.updateSize();
			}
			Header {
				id: header
				table: base
			}
		}
		Controls.ScrollView {
			id: scrollView
			property var table: table

			Layout.row       : 1
			Layout.column    : 1
			Layout.fillWidth : true
			Layout.fillHeight: true

			TableView {
				id: table
				clip: true

				property bool hasColumnHeaders: table.model.headerData(-1, Qt.Horizontal) !== 0
				property bool hasRowHeaders   : table.model.headerData(-1, Qt.Vertical  ) !== 0

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
					leftBorder : !base.fillsParent ? column === 0 : false
					rightBorder: (base.fillsParent && column === table.columns - 1) ? table.remainWidth > 0 : true
				}

				columnWidthProvider: base.defaultColumnWidthProvider
				rowHeightProvider  : base.defaultRowHeightProvider

				property real remainHeight: base.height - table.contentHeight - base.scrollHeight
				property real remainWidth : base.width  - table.contentWidth  - base.scrollWidth

				pixelAligned: true

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
							leftBorder  : !base.fillsParent ? modelData === 0 : false
							bottomBorder: !base.fillsParent
							rightBorder : (base.fillsParent && modelData === table.columns - 1) ? table.remainWidth > 0 : true

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

						bottomBorder: !base.fillsParent
						rightBorder : !base.fillsParent
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
		}
	}

	component Item : 
	Controls.Control {
		property alias text: column.text
		property alias horizontalAlignment: column.horizontalAlignment

		property bool topBorder   : false
		property bool leftBorder  : false
		property bool bottomBorder: true
		property bool rightBorder : true
		
		height: column.height

		ColumnLayout {
			id: column

			anchors.fill: parent

			height: row.height + _bottomBorder.height

			property alias text: row.text
			property alias horizontalAlignment: row.horizontalAlignment

			spacing: 0

			Kirigami.Separator {
				id: _topBorder
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
				id: _bottomBorder
				Layout.margins  : 0
				Layout.fillWidth: true
				visible: bottomBorder
			}
		}
	}

	component Header : Flickable {
		id: scroll
		property var table
		property var _base: null

		Component.onCompleted: {
			if (table.table !== undefined) {
				_base = table;
				table = _base.table;
			}
		}

		interactive: false
		contentX: table.contentX
		contentHeight: row.height
		height: row.height
		
		onWidthChanged: {
			updateSize();
		}
		signal updateSize()
		
		Connections {
			target: _base
			function onScrollWidthChanged() {
				scroll.updateSize();
			}
			function onScrollHeightChanged() {
				scroll.updateSize();
			}
		}

		Row {
			id: row

			Repeater {
				model: table.columns >= 0 ? table.columns : 0
				Item {
					id: headerItem

					width: table.columnWidthProvider(modelData)
					text : table.model.headerData(modelData, Qt.Horizontal)
					topBorder   : _base.showHeaders && !_base.fillsParent
					leftBorder  : !_base.fillsParent ? modelData === 0 : false
					bottomBorder: _base.showHeaders
					rightBorder : _base.fillsParent && modelData === table.columns - 1 ? table.remainWidth > 0 : true

					Connections {
						target: table.model
						function onDataChanged() {
							headerItem.width = table.columnWidthProvider(modelData);
						}
					}
					Connections {
						target: scroll
						function onUpdateSize() {
							headerItem.width = table.columnWidthProvider(modelData);
							
						}
					}
				}
			}
			Item {
				width: (table.remainWidth > 0 ? table.remainWidth : 0) + _base.scrollWidth
				visible: (table.remainWidth > 0 && _base.showHeaders) || _base.scrollWidth > 0

				topBorder   : _base.showHeaders && !_base.fillsParent
				bottomBorder: _base.showHeaders
				rightBorder : !_base.fillsParent
			}
		}
	}
}