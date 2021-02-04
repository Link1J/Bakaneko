// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.14
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.0
import org.kde.kirigami 2.8 as Kirigami
import QtQuick.Controls 2.8 as Controls
import Bakaneko.Models 1.0 as Models
import Bakaneko.Components 1.0 as Components
import Bakaneko.Dialogs 1.0 as Dialogs

Kirigami.Page {
	Kirigami.Theme.colorSet: Kirigami.Theme.View

	id: page
	title: currentServer.serviceManager
	padding: 0
	
	header:
	Controls.ToolBar {
		property alias comboBox: row.comboBox
		property alias searchField: row.searchField

		RowLayout {
			id: row

			property var comboBox: comboBox
			property var searchField: searchField

			anchors.fill: parent

			Controls.ComboBox {
				id: comboBox

				Layout.maximumWidth: 100 * Kirigami.Units.devicePixelRatio

				model: currentServer.serviceTypes
				textRole: "display"

				onActivated: {
					table.forceLayout()
				}
			}
			Kirigami.SearchField {
				id: searchField
				Layout.fillWidth: true

				onTextChanged: {
					table.forceLayout()
				}
			}
		}
	}

	Components.Table {
		id: table
		anchors.fill: parent

		columnWidthProvider: function (column) {
			if (column === 3) {
				return dynamic_column(3);
			}
			return defaultColumnWidthProvider(column);
		}

		rowHeightProvider: function (row) {
			if (header.comboBox.currentText !== "All") {
				if (header.comboBox.currentText !== model.data(model.index(row, 2), Qt.DisplayRole)) {
					return 0;
				}
			}
			if (!model.fuzzy_check(row, header.searchField.text)) {
				return 0;
			}
			return defaultRowHeightProvider(row);
		}

		model: currentServer.services

		delegate:
		Components.Table.Item {
			id: item
			visible: table.contentHeight !== 0
			text: display
			hoverEnabled: true
			leftBorder : !table.fillsParent ? column === 0 : false
			rightBorder: (table.fillsParent && column === table.columns - 1) ? table.table.remainWidth > 0 : true

			TableView.onPooled: {
				mouseArea.contextMenu.visible = false
				mouseArea.contextMenu.dismiss()
			}
			TableView.onReused: {
			}

			Controls.ToolTip {
				parent: item
				visible: item.hovered && tooltip !== ""
				text: tooltip
			}
			MouseArea {
				id: mouseArea

				anchors.fill: parent
				acceptedButtons: Qt.LeftButton | Qt.RightButton
				onClicked: {
					if (mouse.button === Qt.RightButton)
						contextMenu.popup()
				}
				onPressAndHold: {
					if (mouse.source === Qt.MouseEventNotSynthesized)
						contextMenu.popup()
				}

				property var contextMenu: contextMenu

				Controls.Menu {
					id: contextMenu
					Controls.MenuItem {
						visible: service_state == 0
						text: "Start"
						onTriggered: {
							login_prompt1.createObject(overlay, { source_page: page }).open();
						}
						Component {
							id: login_prompt1
							Dialogs.Login {
								onRun: function(logindata) {
									currentServer.control_service(logindata, service_id, 1);
								}
							}
						}
					}
					Controls.MenuItem {
						visible: service_state == 1
						text: "Stop"
						onTriggered: {
							login_prompt2.createObject(overlay, { source_page: page }).open();
						}
						Component {
							id: login_prompt2
							Dialogs.Login {
								onRun: function(logindata) {
									currentServer.control_service(logindata, service_id, 0);
								}
							}
						}
					}
					Controls.MenuItem {
						visible: service_state == 1
						text: "Restart"
						onTriggered: {
							login_prompt3.createObject(overlay, { source_page: page }).open();
						}
						Component {
							id: login_prompt3
							Dialogs.Login {
								onRun: function(logindata) {
									currentServer.control_service(logindata, service_id, 2);
								}
							}
						}
					}

					Controls.MenuSeparator {}

					Controls.MenuItem {
						visible: !service_enabled
						text: "Enable"
						onTriggered: {
							login_prompt4.createObject(overlay, { source_page: page }).open();
						}
						Component {
							id: login_prompt4
							Dialogs.Login {
								onRun: function(logindata) {
									currentServer.control_service(logindata, service_id, 3);
								}
							}
						}
					}
					Controls.MenuItem {
						visible: service_enabled
						text: "Disable"
						onTriggered: {
							login_prompt5.createObject(overlay, { source_page: page }).open();
						}
						Component {
							id: login_prompt5
							Dialogs.Login {
								onRun: function(logindata) {
									currentServer.control_service(logindata, service_id, 4);
								}
							}
						}
					}
				}
			}
		}
	}
}