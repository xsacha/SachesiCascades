import bb.cascades 1.3
import bb.cascades.pickers 1.0
import custom.pickers 1.0

Page {
    id: scanPage
    objectName: "scanner"
    titleBar: TitleBar {
        title: qsTr("Version Scanner") + Retranslate.onLocaleOrLanguageChanged
    }
    actions: [
        ActionItem {
            title: qsTr("Lookup") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///search.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: scanner.reverseLookup(osPicker.selectedValue)
        },
        ActionItem {
            title: qsTr("Autoscan") + Retranslate.onLocaleOrLanguageChanged
            enabled: scanner.historyCount
            imageSource: "asset:///radar.png"
            backgroundColor: scanner.isAuto ? ui.palette.primaryBase : ui.palette.plainBase
            property bool finished: scanner.finishedScan
            onFinishedChanged: {
                if (finished && !scanner.isActive && scanner.isAuto) {
                    scanner.reverseLookup(osPicker.incBuild())
                }
            }
            onTriggered: {
                scanner.isAuto = !scanner.isAuto;
                if (scanner.isAuto) {
                    scanner.reverseLookup(osPicker.incBuild())
                }
            }
            ActionBar.placement: ActionBarPlacement.Signature
        },
        ActionItem {
            title: qsTr("History") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///history.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                var page = historyDefinition.createObject();
                navPane.push(page);
            }
        }
    ]
    attachedObjects: [
        ComponentDefinition {
            id: historyDefinition;
            source: "ScannerHistory.qml"
        }
    ]
    Container {
        Header {
            title: qsTr("Stop on:")
        }
        SegmentedControl {
            options: [
                Option {
                    text: qsTr("Next Found")
                },
                Option {
                    text: qsTr("Next Existing Links")
                },
                Option {
                    text: qsTr("Never")
                }
            ]
            onSelectedIndexChanged: if (scanner.findExisting != selectedIndex) { scanner.findExisting = selectedIndex }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Picker {
                id: osPicker
                verticalAlignment: VerticalAlignment.Center
                title: qsTr("OS Version") + Retranslate.onLocaleOrLanguageChanged
                property string latestOS: searchPage.latestOS
                onCreationCompleted: latestOSChanged()
                onLatestOSChanged: {
                    var array = latestOS.split('.')
                    if (columnCount > 3 && array.length > 3) {
                        var major = parseInt(array[1])
                        var minor = parseInt(array[2])
                        var build = parseInt(array[3])
                        select(0, major);
                        select(1, 5 + minor);
                        select(2, (build / 100) % 100)
                        select(3, 10 + (build / 10) % 10)
                        select(4, 10 + build % 10)
                    }
                }
                description: selectedValue
                function decBuild() {
                    var osString = selectedValue
                    var oldfourth = selectedIndex(4) % 10;
                    select(4, 10 + ((oldfourth + (10-3)) % 10), ScrollAnimation.Smooth)
                    if (oldfourth < 3) {
                        var oldthird = selectedIndex(3) % 10;
                        select(3, 10 + ((oldthird + (10-1)) % 10), ScrollAnimation.Smooth)
                        if (oldthird < 1) {
                            var oldsecond = selectedIndex(2);
                            select(2, (oldsecond + (50-1)) % 50, ScrollAnimation.Smooth)
                        }
                    }
                    var components = osString.split('.')
                    components[3] = parseInt(components[3], 10) - 3
                    osString = components.join('.')
                    return osString
                }
                function incBuild() {
                    var osString = selectedValue
                    var oldfourth = selectedIndex(4) % 10;
                    select(4, 10 + ((oldfourth + 3) % 10), ScrollAnimation.Smooth)
                    if (oldfourth >= 7) {
                        var oldthird = selectedIndex(3) % 10;
                        select(3, 10 + ((oldthird + 1) % 10), ScrollAnimation.Smooth)
                        if (oldthird >= 9) {
                            var oldsecond = selectedIndex(2);
                            select(2, (oldsecond + 1) % 50, ScrollAnimation.Smooth)
                        }
                    }
                    var components = osString.split('.')
                    components[3] = parseInt(components[3], 10) + 3
                    osString = components.join('.')
                    return osString
                }
                pickerItemProvider: OSPickerProvider {
                    columnWidthRatio: [5, 2, 5, 5, 5]
                }
            }
            Container {
                Button {
                    text: "+"
                    preferredWidth: ui.du(7.0)
                    onClicked: {
                        osPicker.incBuild();
                    }
                }
                Button {
                    text: "-"
                    preferredWidth: ui.du(7.0)
                    onClicked: {
                        osPicker.decBuild();
                    }
                }
                margin.leftOffset: ui.du(1.0)
            }
            margin {
                topOffset: ui.du(2.0)
                leftOffset: ui.du(1.0)
                rightOffset: ui.du(1.0)
            }
        }
        Label {
            visible: scanner.isActive
            text: qsTr("Scanning...") + Retranslate.onLocaleOrLanguageChanged
            textStyle.fontSize: FontSize.XXLarge
            horizontalAlignment: HorizontalAlignment.Center
        }
        Label {
            visible: scanner.curRelease != null && scanner.curRelease.srVersion != ""
            text: qsTr("Found: ") + (scanner.curRelease != null ? scanner.curRelease.srVersion : "") + Retranslate.onLocaleOrLanguageChanged
            textStyle.fontSize: FontSize.XLarge
            horizontalAlignment: HorizontalAlignment.Center
            margin {
                topOffset: ui.du(1.0)
                bottomOffset: ui.du(1.0)
            }
        }
        /*Label {
         visible: scanner.curRelease != null && scanner.curRelease.srVersion != ""
         text: "(" + qsTr("OS: ") + (scanner.curRelease != null ? scanner.curRelease.osVersion : "") + ")" + Retranslate.onLocaleOrLanguageChanged
         textStyle.fontSize: FontSize.Large
         horizontalAlignment: HorizontalAlignment.Center 
         }*/
Label {
            visible: scanner.curRelease != null && scanner.curRelease.srVersion != ""
            text: {
                var ret = ""
                if (scanner.curRelease != null) {
                    if (scanner.curRelease.activeServers & 1)
                        ret += qsTr("Production") + " "
                    if (scanner.curRelease.activeServers & 2)
                        ret += qsTr("Beta") + " "
                    if (scanner.curRelease.activeServers & 4)
                        ret += qsTr("Alpha") + " "
                    if (ret.length > 0)
                        ret = qsTr("Servers: ") + ret + Retranslate.onLocaleOrLanguageChanged;
                }
                return ret;
            }
            textStyle.fontSize: FontSize.Large
            horizontalAlignment: HorizontalAlignment.Center
}
Button {
    visible: scanner.curRelease != null && scanner.curRelease.baseUrl != ""
    text: qsTr("Generate Links") + Retranslate.onLocaleOrLanguageChanged
    onClicked: scanner.generatePotentialLinks()
    horizontalAlignment: HorizontalAlignment.Center
}
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: navPane.pop();
        }
    }
}
