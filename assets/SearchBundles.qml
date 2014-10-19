import bb.cascades 1.3

Page {
    id: bundlesPage
    titleBar: TitleBar {
        title: qsTr("Available Bundles") + Retranslate.onLocaleOrLanguageChanged
    }
    actions: [
        ActionItem {
            title: qsTr("Latest") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///star.png"
            enabled: bundleListView.dataModel.hasChildren(0)
            onTriggered: bundleListView.triggered([0])
            ActionBar.placement: ActionBarPlacement.Signature
        }
    ]
    attachedObjects: [
        ComponentDefinition {
            id: resultsDefinition;
            source: "SearchResults.qml"
        }
    ]
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {}
        background: ui.palette.background
        ActivityIndicator {
            running: search.scanning
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
        }
        ListView {
            id: bundleListView
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            visible: !search.scanning
            dataModel: search.bundleList
            topPadding: -ui.du(3.0)
            margin {
                leftOffset: ui.du(1.0)
                rightOffset: ui.du(1.0)
            }
            listItemComponents: [
                ListItemComponent {
                    Container {
                        id: bundleDelegate
                        topPadding: ui.du(3.0)
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        Label {
                            text: ListItemData.name + " | " + ListItemData.device
                            horizontalAlignment: HorizontalAlignment.Center
                            textStyle.fontSize: FontSize.XXLarge
                        }
                    }
                }
            ]
            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                search.fromBundleRequest(carrierPicker.selectedValue, countryPicker.selectedValue, chosenItem.deviceId, chosenItem.variantId, chosenItem.name)
                var page = resultsDefinition.createObject();
                navPane.push(page);
            }
        }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: navPane.pop();
        }
    }
}
