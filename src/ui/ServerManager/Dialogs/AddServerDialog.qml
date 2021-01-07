import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.15 as Controls
import ServerManager.Objects 1.0 as Objects
import ServerManager.Managers 1.0 as Managers

Kirigami.OverlaySheet {
    id: addServerDialog

    parent: applicationWindow().overlay

    header: Kirigami.Heading {
        text: i18n("Add a Server")
    }
    contentItem: Kirigami.FormLayout {
        Controls.TextField {
            id: ip
            Kirigami.FormData.label: i18n("Server IP")
            onAccepted: username.forceActiveFocus();
        }
        Controls.TextField {
            id: username
            Kirigami.FormData.label: i18n("Username")
            onAccepted: password.forceActiveFocus();
        }
        Controls.TextField {
            id: password
            Kirigami.FormData.label: i18n("Password")
            onAccepted: addButton.forceActiveFocus();
            echoMode: TextInput.PasswordEchoOnEdit
        }
        Controls.Button {
            id: addButton
            text: i18nc("@action:button", "Add")
            onClicked: {
                Managers.Server.AddServer(ip.text, username.text, password.text);
                close()
            }
        }
    }
}