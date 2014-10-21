import bb.cascades 1.3

Page {
    id: historyPage
    titleBar: TitleBar {
        title: qsTr("Scan History") + Retranslate.onLocaleOrLanguageChanged
    }
    actions: [
        ActionItem {
            title: qsTr("Clear") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: scanner.clearHistory();
            ActionBar.placement: ActionBarPlacement.Signature
        }
    ]
    Container {
        layout: DockLayout {}
        Label {
            visible: scanner.history.count > 0
            text: qsTr("No history") + Retranslate.onLocaleOrLanguageChanged
            textStyle.fontSize: FontSize.XXLarge
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
        }
        ListView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            dataModel: scanner.history
            listItemComponents: [
                ListItemComponent {
                    StandardListItem {
                        title: qsTr("SR: ") + ListItemData.srVersion + " | " + qsTr("OS: ") + ListItemData.osVersion
                        + " [" + (ListItemData.activeServers & 1 ? qsTr("Production") + " " : "") + (ListItemData.activeServers & 2 ? qsTr("Beta") + " " : "") + (ListItemData.activeServers & 4 ? qsTr("Alpha") + " " : "") + "]" + Retranslate.onLocaleOrLanguageChanged
                    }
                }
            ]
        }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: navPane.pop();
        }
    }
}
