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
            onTriggered: search.reverseLookup(osPicker.selectedValue, findExisting.checked)
        },
        ActionItem {
            title: qsTr("Autoscan") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///radar.png"
            backgroundColor: ui.palette.plainBase
            property string swRelease: search.softwareRelease
            onSwReleaseChanged: {
                if (!search.scanning && search.softwareRelease != "") {
                    if (search.autoscan && search.softwareRelease.indexOf("10") != 0) {
                        search.reverseLookup(osPicker.incBuild(), findExisting.checked)
                    }
                    osPicker.lastWorkingOS = osPicker.selectedValue
                }
            }
            onTriggered: { search.autoscan = !search.autoscan; search.reverseLookup(osPicker.incBuild(), findExisting.checked) }
            ActionBar.placement: ActionBarPlacement.Signature
        }
    ]
    Container {
            Picker {
                id: osPicker
                title: qsTr("OS Version") + Retranslate.onLocaleOrLanguageChanged
                property string lastWorkingOS: ""
                property int initialValue: 301418
                description: selectedValue
                function decBuild() {
                    var osString = selectedValue
                    var oldfourth = selectedIndex(4) % 10;
                    select(4, 10 + ((oldfourth + (10-3)) % 10), ScrollAnimation.Smooth)
                    if (oldfourth < 3) {
                        var oldthird = selectedIndex(3) % 10;
                        select(3, 10 + ((oldthird + (10-1)) % 10), ScrollAnimation.Smooth)
                        if (oldthird >= 9) {
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
                onCreationCompleted: {
                    select(0, initialValue / 100000);
                    select(1, 5 + initialValue / 10000 % 10);
                    select(2, initialValue / 100 % 100);
                    select(3, 10 + initialValue / 10 % 10);
                    select(4, 10 + initialValue % 10);
                }
                pickerItemProvider: OSPickerProvider {
                    columnWidthRatio: [5, 2, 5, 5, 5]
                }
                margin {
                    topOffset: ui.du(2.0)
                    leftOffset: ui.du(1.0)
                    rightOffset: ui.du(1.0)
                }
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Button {
                    text: "-"
                    onClicked: {
                        osPicker.decBuild();
                    }
                }
                Button {
                    text: "+"
                    onClicked: {
                        osPicker.incBuild();
                    }
                }
            }
                CheckBox {
                    id: findExisting
                    text: qsTr("Find only existing links") + Retranslate.onLocaleOrLanguageChanged
                    margin {
                        leftOffset: ui.du(1.0)
                        rightOffset: ui.du(1.0)
                    }
                }
                Label {
                    visible: search.scanning
                    text: qsTr("Checking...")
                    textStyle.fontSize: FontSize.XXLarge
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    visible: !search.scanning && search.softwareRelease != ""
                    text: {
                        if (search.softwareRelease.indexOf("10") == 0)
                            return qsTr("Found: ") + search.softwareRelease
                        else if (search.softwareRelease.indexOf("SR") == 0)
                            return qsTr("Release not found")
                        else
                            return search.softwareRelease
                    }
                    textStyle.fontSize: FontSize.XXLarge
                    horizontalAlignment: HorizontalAlignment.Center
                    margin {
                        topOffset: ui.du(1.0)
                        bottomOffset: ui.du(1.0)
                    }
                }
                Label {
                    visible: search.softwareRelease.indexOf("10") == 0 || search.softwareRelease.indexOf("SR") == 0
                    text: "(" + qsTr("OS: ") + osPicker.lastWorkingOS + ")"
                    textStyle.fontSize: FontSize.XLarge
                    horizontalAlignment: HorizontalAlignment.Center 
                }
                Button {
                    visible: search.hasPotentialLinks && search.softwareRelease.indexOf("10") == 0
                    text: qsTr("Generate Links")
                    onClicked: search.generatePotentialLinks(osPicker.lastWorkingOS)
                    horizontalAlignment: HorizontalAlignment.Center
                }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: navPane.pop();
        }
    }
}
