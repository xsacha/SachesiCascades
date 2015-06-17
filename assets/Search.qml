import bb.cascades 1.3
import bb.cascades.pickers 1.0
import org.labsquare 1.0
import custom.pickers 1.0

NavigationPane {
    id: navPane
    Page {
        id: searchPage
        titleBar: TitleBar {
            title: qsTr("Update Search") + Retranslate.onLocaleOrLanguageChanged
        }
        actions: [
            ActionItem {
                title: qsTr("Latest Update") + Retranslate.onLocaleOrLanguageChanged
                imageSource: "asset:///star.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    search.updateDetailRequest(carrierPicker.selectedValue, countryPicker.selectedValue, deviceSelector.selectedIndex)
                    var page = resultsDefinition.createObject();
                    navPane.push(page);
                }
            },
            ActionItem {
                title: qsTr("Search") + Retranslate.onLocaleOrLanguageChanged
                imageSource: "asset:///search.png"
                ActionBar.placement: ActionBarPlacement.Signature
                backgroundColor: ui.palette.plainBase
                onTriggered: {
                    search.availableBundleRequest(carrierPicker.selectedValue, countryPicker.selectedValue, deviceSelector.selectedIndex)
                    var page = bundlesDefinition.createObject();
                    navPane.push(page);
                }
            },
            ActionItem {
                title: qsTr("Scanner") + Retranslate.onLocaleOrLanguageChanged
                imageSource: "asset:///radar.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    var page = scannerDefinition.createObject();
                    navPane.push(page);
                }
            }
        ]
        attachedObjects: [
            ComponentDefinition {
                id: bundlesDefinition;
                source: "SearchBundles.qml"
            },
            ComponentDefinition {
                id: resultsDefinition;
                source: "SearchResults.qml"
            },
            ComponentDefinition {
                id: scannerDefinition;
                source: "SearchScanner.qml"
            }
        ]
        property string latestOS: "10.3.2.840"
        onCreationCompleted: {
            var http = new XMLHttpRequest()
            var url = "https://raw.githubusercontent.com/xsacha/Sachesi/master/carrier";
            http.open("GET", url, true);
            http.send(null)
            http.onreadystatechange = function() {
                if(http.readyState == 4 && http.status == 200) {
                    var array = http.responseText.split('\n')
                    if (array.length > 3) {
                        var countrycode = parseInt(array[0])
                        countryPicker.select(0, 10 + countrycode / 100);
                        countryPicker.select(1, 10 + countrycode / 10 % 10);
                        countryPicker.select(2, 10 + countrycode % 10);
                        var carriercode = parseInt(array[1])
                        carrierPicker.select(0, 10 + carriercode / 100);
                        carrierPicker.select(1, 10 + carriercode / 10 % 10);
                        carrierPicker.select(2, 10 + carriercode % 10);
                        deviceSelector.selectedIndex = parseInt(array[2])
                        latestOS = array[3]
                    }
                }
            }
        }
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            Header {
                title: qsTr("Whitelist Settings")
            }
            Picker {
                id: countryPicker
                title: qsTr("Country") + Retranslate.onLocaleOrLanguageChanged
                description: carrier.country == "" ? (qsTr("Unknown") + Retranslate.onLocaleOrLanguageChanged) : carrier.country
                property int initialValue: 311
                onSelectedValueChanged: carrier.mccChange(selectedValue)
                onCreationCompleted: {
                    select(0, 10 + initialValue / 100);
                    select(1, 10 + initialValue / 10 % 10);
                    select(2, 10 + initialValue % 10);
                }
                pickerItemProvider: CodePickerProvider {
                    columnWidthRatio: [1, 1, 1]
                }
                margin {
                    topOffset: ui.du(2.0)
                    leftOffset: ui.du(1.0)
                    rightOffset: ui.du(1.0)
                }
            }
            Picker {
                id: carrierPicker
                title: qsTr("Carrier") + Retranslate.onLocaleOrLanguageChanged
                description: carrier.carrier == "" ? (qsTr("Unknown") + Retranslate.onLocaleOrLanguageChanged) : carrier.carrier
                property int initialValue: 480
                onSelectedValueChanged: carrier.mncChange(selectedValue)
                onCreationCompleted: {
                    select(0, 10 + initialValue / 100);
                    select(1, 10 + initialValue / 10 % 10);
                    select(2, 10 + initialValue % 10);
                }
                pickerItemProvider: CodePickerProvider {
                    columnWidthRatio: [1, 1, 1]
                }
                margin {
                    topOffset: ui.du(2.0)
                    leftOffset: ui.du(1.0)
                    rightOffset: ui.du(1.0)
                }
            }
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                background: ui.palette.plainBase
                WebImageView {
                    preferredWidth: ui.du(30.0)
                    scalingMethod: ScalingMethod.AspectFit
                    url: carrier.image == 0 ? "" : "http://appworld.blackberry.com/ClientAPI/image/" + carrier.image + "/150X/png"
                    gestureHandlers: [
                        TapHandler {
                            onTapped: Qt.openUrlExternally("https://en.wikipedia.org/w/index.php?title=Mobile_country_code#A")
                        }
                    ]
                }
            }
            DropDown {
                id: deviceSelector
                selectedIndex: 0
                title: qsTr("Search Device") + Retranslate.onLocaleOrLanguageChanged
                options: [
                    Option {
                        text: "Z30 + Classic + Leap"
                    },
                    Option {
                        text: "Z10 STL 100-1"
                    },
                    Option {
                        text: "Z10 STL 100-2/3/4"
                    },
                    Option {
                        text: "Z3"
                    },
                    Option {
                        text: "Passport"
                    },
                    Option {
                        text: "Q5 + Q10"                
                    }
                ]
                margin {
                    topOffset: ui.du(2.0)
                    leftOffset: ui.du(1.0)
                    rightOffset: ui.du(1.0)
                }
            }
        }
    }
    onPopTransitionEnded: {
        // If Scanner gets popped while autoscanning, turn it off?
        if (page.objectName == "scanner")
            scanner.isAuto = false
        page.destroy();
    }
}