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
            enabled: scanner.historyCount
            ActionBar.placement: ActionBarPlacement.Signature
        },
        ActionItem {
            title: qsTr("Export") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///history.png"
            enabled: scanner.historyCount
            onTriggered: scanner.exportHistory();
            ActionBar.placement: ActionBarPlacement.OnBar
        }
    ]
    attachedObjects: [
        Invocation {
            id: invokeShare
            query: InvokeQuery {
                id: invokeQuery
                mimeType: "text/plain; charset=utf-8"
            }
            onArmed: {
                if (invokeQuery.data != "") {
                    trigger("bb.action.SHARE");
                }
            }
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
            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                invokeQuery.setData(world.utf8Encode(qsTr("I found ") + qsTr("OS: ") + chosenItem.osVersion + qsTr(" with ") + qsTr("SR: ") + chosenItem.srVersion + qsTr(" using Sachesi ")));
                invokeQuery.updateQuery();
            }
        }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: navPane.pop();
        }
    }
}
