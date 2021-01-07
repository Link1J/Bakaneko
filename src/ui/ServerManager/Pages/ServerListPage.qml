import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.15 as Controls
import ServerManager.Pages 1.0 as Pages
import ServerManager.Models 1.0 as Models
import ServerManager.Components 1.0 as Components
import ServerManager.Dialogs 1.0 as Dialogs

Kirigami.ScrollablePage {
    title: i18n("Server List")

    Kirigami.Action {
        id: serverAddAction
        icon.name: "list-add"
        text: i18n("Add Server")
        onTriggered: {
            addServerDialog.createObject(overlay).open();
        }
    }

    actions {
        right: serverAddAction
    }

    ListView {
        id: listView
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            visible: listView.count === 0
            text: i18n("No Servers")
            helpfulAction: serverAddAction
        }
        delegate: Kirigami.SwipeListItem {
            highlighted: currentServer && currentServer.hostname === hostname

            contentItem: Components.ServerListItem {}

            actions: [
                 Kirigami.Action {
                     icon.name: "list-remove"
                     onTriggered: print("Action 1 clicked")
                 }
            ]
        }
        model: Models.ServerList {}
    }

    Component {
        id: addServerDialog
        Dialogs.AddServer {}
    }
    Component {
        id: serverpage
        Pages.ServerList {}
    }
}