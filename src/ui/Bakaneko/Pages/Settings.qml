// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

import QtQuick 2.1
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.0 as Controls
import Bakaneko.Objects 1.0 as Objects
import Bakaneko.Managers 1.0 as Managers

Kirigami.ScrollablePage
{
	title: i18n("Settings")

	Kirigami.FormLayout {
		Kirigami.Separator {
			Kirigami.FormData.label: i18n("Wake on LAN")
			Kirigami.FormData.isSection: true
		}
		Controls.SpinBox {
			Controls.ToolTip.text: i18n("UDP to send the magic packet.\nIf unsure leave at 9.")
			Controls.ToolTip.timeout: -1
			Kirigami.FormData.label: i18n("Port")
			from: 0
			to: 65535
			editable: true
			value: Managers.Settings.wol_port
			onValueModified: Managers.Settings.wol_port = value
		}
		Kirigami.Separator {
			Kirigami.FormData.label: i18n("Server")
			Kirigami.FormData.isSection: true
		}
		Controls.Slider {
			// Controls.ToolTip.text: i18n("\nIf unsure leave at 5.")
			// Controls.ToolTip.timeout: -1
			Kirigami.FormData.label: i18n("Refresh Rate")
			from: 1
			to: 10
			stepSize: 1.0
			value: Managers.Settings.server_refresh_rate
			onMoved: Managers.Settings.server_refresh_rate = value
		}
		Kirigami.Separator {
			Kirigami.FormData.label: i18n("Terminal")
			Kirigami.FormData.isSection: true
		}
		RowLayout {
			Kirigami.FormData.label: i18n("Font")

			Controls.TextField {
				readOnly: true
				text: Managers.Settings.font.family + " " + Managers.Settings.font.pointSize + "pt"
				font: Managers.Settings.font
			}
			Controls.Button {
				Layout.fillHeight: true
				icon.name: "document-edit"
				onClicked: {
					fontDialog.open();
				}
			}
			FontDialog {
				id: fontDialog
				title: "Please choose a font"
				font: Managers.Settings.font
				modality: Qt.Modal
				monospacedFonts: true
				onAccepted: {
					Managers.Settings.font = font
					close();
				}
				onRejected: {
					close()
				}
			}
		}
	}
}