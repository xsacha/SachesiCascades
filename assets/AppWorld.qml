import bb.cascades 1.3
import org.labsquare 1.0

Page {
    actions: [
        TextInputActionItem {
            id: inputAction
            text: ""
            hintText: qsTr("Search") + Retranslate.onLocaleOrLanguageChanged
            input.keyLayout: KeyLayout.Text
            input {
                submitKey: SubmitKey.Send
                onSubmitted: world.search(inputAction.text)
            }
        },
        ActionItem {
            title: qsTr("Lookup") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: world.lookup()
            imageSource: "asset:///search.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: qsTr("Home") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: world.showHome()
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: qsTr("Featured") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: world.searchLocker("featured")
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: qsTr("Carrier") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: world.searchLocker("mypreloadeditems")
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: qsTr("Cars") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: world.showCars()
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        DeleteActionItem {
            id: backAction
            title: qsTr("Back") + Retranslate.onLocaleOrLanguageChanged
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///back.png"
            enabled: !world.listing
            onTriggered: world.listing = true
        }
    ]
    attachedObjects: [      
        Invocation {
            id: invokeShare
            query: InvokeQuery {
                id:invokeQuery
                mimeType: "text/plain"                        
            }
            onArmed: {
                if (invokeQuery.data != "") {
                    trigger("bb.action.SHARE");
                }
            }             
        }
    ]
    Container {
        id: listContainer
        
        layout: DockLayout {}
        // No content
        ActivityIndicator {
            id: contentIndicator
            running: !world.listing && world.contentItem.id == ""
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
        }
        attachedObjects: [
            LayoutUpdateHandler {
                onLayoutFrameChanged: {
                    contentIndicator.preferredWidth  = layoutFrame.width;
                    contentIndicator.preferredHeight = layoutFrame.height;
                }
            }
        ]
        // Showing content id
        ScrollView {
            visible: !world.listing && world.contentItem.id != ""
            focusPolicy: FocusPolicy.Touch
            onVisibleChanged: {
                if (visible) scrollToPoint(0, 0, ScrollAnimation.None)
            }
            scrollViewProperties {
                scrollMode: ScrollMode.Vertical
            }
            Container {
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    WebImageView {
                        url: world.contentItem.image == "" ? "" : world.contentItem.image + "/X96/png"
                    }
                    Label {
                        verticalAlignment: VerticalAlignment.Center
                        text: world.contentItem.friendlyName
                        textFit.minFontSizeValue: 12
                        textFit.maxFontSizeValue: 100
                        textFit.mode: LabelTextFitMode.FitToBounds
                    }
                }
                Label {
                    textFormat: TextFormat.Html
                    text: qsTr("by") + " <b><a href=\"qtness://\">" + world.contentItem.vendor.replace("&","+") + "</a></b>" + Retranslate.onLocaleOrLanguageChanged
                    activeTextHandler: ActiveTextHandler {
                        onTriggered: world.showVendor(world.contentItem.vendorId);
                    }
                }
                Label {
                    id: bundleText
                    visible: world.contentItem.name != ""
                    textFormat: TextFormat.Html
                    text: "<b>" + qsTr("File Bundle") + "</b>: " + world.contentItem.name + ".bar" + Retranslate.onLocaleOrLanguageChanged
                    textStyle.fontSize: FontSize.Large
                }
                Label {
                    visible: world.contentItem.size != 0
                    textFormat: TextFormat.Html
                    text: "<b>" + qsTr("Version") + "</b>: " + world.contentItem.version + " [" + (world.contentItem.size / 1024 / 1024).toFixed(2) + " " + qsTr("MB") + "] " + Retranslate.onLocaleOrLanguageChanged
                    textStyle.fontSize: FontSize.Large
                }
                Label {
                    visible: world.contentItem.price != ""
                    textFormat: TextFormat.Html
                    text: "<b>" + qsTr("Price") + "</b>: " + world.contentItem.price + Retranslate.onLocaleOrLanguageChanged
                    textStyle.fontSize: FontSize.Large
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Button {
                        text: qsTr("Share") + Retranslate.onLocaleOrLanguageChanged
                        onClicked: {
                            invokeQuery.mimeType = "text/plain; charset=utf-8"
                            invokeQuery.data = world.utf8Encode(qsTr("I found ") + world.contentItem.friendlyName + qsTr(" using Sachesi ") + "(http://goo.gl/zVzeS3 )! " + qsTr("You can find it with") + " sachesi:" + world.contentItem.id);
                            invokeQuery.updateQuery();
                        }
                    }
                    Button {
                        text: qsTr("Download") + Retranslate.onLocaleOrLanguageChanged
                        onClicked: world.download(world.contentItem.id)
                    }
                }
                TextArea {
                    text: world.contentItem.description
                    editable: false
                }
                ListView {
                    dataModel: world.contentItem.screenshots
                    preferredHeight: ui.du(50.0)
                    layout: FlowListLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    listItemComponents: [
                        ListItemComponent {
                            Container {
                                layout: DockLayout {}
                                WebImageView {
                                    preferredHeight: ui.du(50.0)
                                    url: ListItemData + "/X256/png"
                                    scalingMethod: ScalingMethod.AspectFit
                                }
                            }
                        }
                    ]
                }
            }
        }
        // HACK: This is a hack to get the size of the current font because of stupid Cascades
        Label {
            id: spacerText 
            property int actualHeight: 1
            text: "|"
            translationX: -20
            horizontalAlignment: HorizontalAlignment.Fill
            attachedObjects: [
                LayoutUpdateHandler {
                    onLayoutFrameChanged: spacerText.actualHeight = layoutFrame.height;
                }
            ]
        }
        ListView {
            visible: world.listing
            id: listView
            focusPolicy: FocusPolicy.Touch
            horizontalAlignment: HorizontalAlignment.Fill
            property int actualWidth:  1
            property int actualHeight: 1
            // Let the user manually adjust columns by pinch. Set a sane default based on DPI
            onActualWidthChanged: columns = Math.min(1.0 + listView.actualWidth / ui.du(33.0), 5.0)
            property double columns: 3.0
            gestureHandlers: PinchHandler {
                function updateColumns(event) {
                    listView.columns = Math.min(Math.max((0.8 + event.pinchRatio / 5) * listView.columns, 2.0), 7.0)
                }
                onPinchStarted: updateColumns(event)
                onPinchUpdated: updateColumns(event)
                onPinchEnded: updateColumns(event)
            }
            layout: GridListLayout {
                columnCount: Math.floor(listView.columns)
                verticalCellSpacing: ui.du(2.0)
                horizontalCellSpacing: ui.du(2.0)
                property double cellWidth: (listView.actualWidth / columnCount - ui.du(2.0))
                cellAspectRatio: cellWidth / (cellWidth + spacerText.actualHeight)
            }
            dataModel: world.appList
            listItemComponents: [
                ListItemComponent {
                    Container {
                        id: delegateRoot
                        layout: DockLayout {}
                        ActivityIndicator {
                            running: imageView.loading < 1.0
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                        }
                        Container {
                            id: listDelegate
                            property int actualWidth:  1
                            property int actualHeight: 1
                            layout: DockLayout {}
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                            WebImageView {
                                id: imageView
                                visible: imageView.loading == 1.0
                                loadEffect: ImageViewLoadEffect.FadeZoom
                                url: ListItemData.image == "" ? "" : ListItemData.image + "/128X/png";
                                preferredWidth: listDelegate.actualWidth
                                preferredHeight: preferredWidth
                                verticalAlignment: VerticalAlignment.Top
                                scalingMethod: ScalingMethod.AspectFit
                            }
                            Label {
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: (imageView.visible ? VerticalAlignment.Bottom : VerticalAlignment.Center)
                                textStyle.fontWeight: FontWeight.Bold
                                text: ListItemData.friendlyName
                            }
                            // Cascades doesn't support two objects being stack aligned on bottom dock
                            // But, BBWorld won't approve it if it isn't perfect. So, we have to remove this feature!
                            /*Label {
                             visible: ListItemData.vendor != ""
                             horizontalAlignment: HorizontalAlignment.Center
                             text: "<a href=\"qtness://\">" + ListItemData.vendor.replace("&","+") + "</a>"
                             textFormat: TextFormat.Html
                             activeTextHandler: ActiveTextHandler {
                             onTriggered: {
                             ListItemData.isMarked = true;
                             delegateRoot.ListItem.view.triggered(delegateRoot.ListItem.indexPath)
                             }
                             }
                             }*/

                            attachedObjects: [
                                LayoutUpdateHandler {
                                    onLayoutFrameChanged: {
                                        listDelegate.actualWidth  = layoutFrame.width;
                                        listDelegate.actualHeight = layoutFrame.height;
                                    }
                                }
                            ]
                        }
                    }
                }
            ]
            onTriggered: {
                var chosenItem = dataModel.data(indexPath);
                if (chosenItem.isMarked) {
                    world.showVendor(chosenItem.vendorId);
                    chosenItem.isMarked = false;
                }
                else if (chosenItem.type == "category")
                    world.searchMore(chosenItem.id, true);
                else
                    world.showContentItem(chosenItem.id);
            }
            attachedObjects: [
                LayoutUpdateHandler {
                    onLayoutFrameChanged: {
                        listView.actualWidth  = layoutFrame.width;
                        listView.actualHeight = layoutFrame.height;
                    }
                }
            ]
        }
    }
}
