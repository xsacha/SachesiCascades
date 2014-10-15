import bb.cascades 1.3

Page {
    id: resultPage
    property int scanActive: search.scanning
    onScanActiveChanged: {
        if (search.scanning == 0 && search.updateMessage == "")
            navPane.pop();
    }
    titleBar: TitleBar {
        title: (search.scanning ? qsTr("Searching") : qsTr("Search Results")) + Retranslate.onLocaleOrLanguageChanged
    }
    actions: [
        ActionItem {
            title: "Grab Links"
            enabled: search.updateMessage != ""
            onTriggered: search.grabLinks()
            ActionBar.placement: ActionBarPlacement.Signature
        }
    ]
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {}
        background: ui.palette.background
        Label {
            visible: search.error != "Success" && search.error != "";
            text: search.error
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            textStyle.fontSize: FontSize.Large
        }
        ActivityIndicator {
            visible: search.scanning
            running: parent.visible
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
        }
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            TextArea {
                visible: search.updateMessage != ""
                text: search.updateMessage
                editable: false
            }
            ListView {
                visible: search.updateMessage != ""
                dataModel: search.appList
                listItemComponents: [
                    ListItemComponent {
                        Container {
                            id: updateAppDelegate
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            horizontalAlignment: HorizontalAlignment.Fill
                            margin {
                                leftOffset: ui.du(1.0)
                                rightOffset: ui.du(1.0)
                            }
                            property int actualWidth
                            Label {
                                text: ListItemData.friendlyName
                                preferredWidth: updateAppDelegate.actualWidth * 0.60
                            }
                            Label {
                                text: ListItemData.version
                                textStyle.textAlign: TextAlign.Right
                                preferredWidth: updateAppDelegate.actualWidth * 0.22
                            }
                            Label {
                                text: (ListItemData.size / 1024 / 1024).toFixed(1) + " " + qsTr("MB") + Retranslate.onLocaleOrLanguageChanged
                                textStyle.textAlign: TextAlign.Right
                                preferredWidth: updateAppDelegate.actualWidth * 0.18
                            }
                            attachedObjects: [
                                LayoutUpdateHandler {
                                    onLayoutFrameChanged: updateAppDelegate.actualWidth = layoutFrame.width
                                }
                            ]
                        }
                    }
                ]
            }
        }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: navPane.pop();
        }
    }
}
