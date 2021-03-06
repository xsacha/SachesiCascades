import bb.cascades 1.3

MenuDefinition {
    helpAction: HelpActionItem {
        onTriggered: helpDialog.open()
    }
    settingsAction: SettingsActionItem {
        onTriggered: settingsDialog.open()
    }
    attachedObjects: [
        Dialog {
            id: helpDialog
            Container {
                background: ui.palette.background
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                TextArea {
                    editable: false
                    property string titleVersion: Application.applicationName + " " + Application.applicationVersion
                    text: titleVersion + "\n\n"
                    + qsTr("Developed by") + " Sacha Refshauge (2011 - 2014)\n\n"
                    + qsTr("Thanks to everyone who has tested this application and provided feedback") + ".\n\n"
                    + qsTr("If you have any questions or wish to provide feedback, please contact me") + ":\n"
                    + "sacha@qtness.com\n@QtnessDev\nhttp://qtness.com/blog\n\n"
                    + qsTr("As a courtesy, a cross-platform version of this application is also available on almost any platform that that has a compiler") + ". "
                    + qsTr("The cross-platform application is full-featured and includes the ability to extract, search and (un)install firmware. Hence the name, SachESI") + ".\n"
                    + qsTr("I provide a variety of binaries and full source code for your leisure at") + ":\n"
                    + "http://github.com/xsacha/Sachesi/releases\n\n"
                    + qsTr("If you have any contributions to make or issues to raise about the source code, this is the place to do this") + "."
                    + Retranslate.onLocaleOrLanguageChanged
                }
                Button {
                    horizontalAlignment: HorizontalAlignment.Center
                    text: qsTr("Close") + Retranslate.onLocaleOrLanguageChanged
                    onClicked: helpDialog.close()
                }
            }
        },
        Dialog {
            id: settingsDialog
            Container {
                background: ui.palette.background
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                layout: DockLayout {}
                
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    layout: DockLayout {}
                    visible: activeTab.objectName == "searchTab"
                    Label {
                        horizontalAlignment: HorizontalAlignment.Center
                        text: qsTr("Settings") + Retranslate.onLocaleOrLanguageChanged
                        textStyle.fontSize: FontSize.XLarge
                    }
                    DropDown {
                        id: modeSelect
                        title: qsTr("Search Mode") + Retranslate.onLocaleOrLanguageChanged
                        selectedIndex: search.searchMode
                        onSelectedIndexChanged: if (search.searchMode != selectedIndex) search.searchMode = selectedIndex
                        options: [
                            Option {
                                text: qsTr("Upgrade")
                            },
                            Option {
                                text: qsTr("Debrick")
                            }
                        ]
                    }
                }
                Container {
                    visible: activeTab.objectName == "appTab"
                    Label {
                        horizontalAlignment: HorizontalAlignment.Center
                        text: qsTr("Settings") + Retranslate.onLocaleOrLanguageChanged
                        textStyle.fontSize: FontSize.XLarge
                    }
                    DropDown {
                        id: deviceSelect
                        title: qsTr("Device") + Retranslate.onLocaleOrLanguageChanged
                        selectedIndex: world.model
                        onSelectedIndexChanged: if (world.model != selectedIndex) world.model = selectedIndex
                        options: [
                            Option {
                                text: "BlackBerry Passport"
                            },
                            Option {
                                text: "BlackBerry Q10"
                            },
                            Option {
                                text: "BlackBerry Z30"
                            },
                            Option {
                                text: "BlackBerry Z10 Qualcomm GPU"
                            },
                            Option {
                                text: "BlackBerry Z10 Imagination GPU"
                            },
                            Option {
                                text: "BlackBerry PlayBook"
                            }
                        ]
                    }
                    DropDown {
                        id: countrySelect
                        title: qsTr("Country") + Retranslate.onLocaleOrLanguageChanged
                        selectedIndex: 0
                        onSelectedIndexChanged: {
                            if (selectedIndex == 0)
                                world.country = "";
                            else if (world.country != selectedOption.text)
                                world.country = selectedOption.text
                        }
                        options: [
                            Option { text: "None"},
                            Option { text: "Afghanistan" },
                            Option { text: "Albania" },
                            Option { text: "Algeria" },
                            Option { text: "American Samoa" },
                            Option { text: "Andorra" },
                            Option { text: "Australia" },
                            Option { text: "Austria" },
                            Option { text: "Canada" },
                            Option { text: "Colombia" },
                            Option { text: "France" },
                            Option { text: "Hong Kong" },
                            Option { text: "India" },
                            Option { text: "Russia" },
                            Option { text: "Spain" },
                            Option { text: "Poland" },
                            Option { text: "United Kingdom" },
                            Option { text: "United States" },
                            Option { text: "Ukraine" }
                        ]
                    }
                    DropDown {
                        title: qsTr("Server") + Retranslate.onLocaleOrLanguageChanged
                        selectedIndex: world.server
                        onSelectedIndexChanged: if (world.server != selectedIndex) world.server = selectedIndex
                        options: [
                            Option {
                                text: qsTr("Production") + Retranslate.onLocaleOrLanguageChanged
                            },
                            Option {
                                text: qsTr("Enterprise") + Retranslate.onLocaleOrLanguageChanged
                            },
                            Option {
                                text: qsTr("Eval") + Retranslate.onLocaleOrLanguageChanged
                            },
                            Option {
                                text: qsTr("Eval") + "2" + Retranslate.onLocaleOrLanguageChanged
                            },
                            Option {
                                text: "AW46 Dev"
                            },
                            Option {
                                text: "AW10 Dev"
                            },
                            Option {
                                text: "AW33 Dev"
                            }
                        ]
                    }
                    DropDown {
                        title: qsTr("OS") + Retranslate.onLocaleOrLanguageChanged
                        selectedIndex: world.osVer
                        onSelectedIndexChanged: if (world.osVer != selectedIndex) world.osVer = selectedIndex
                        enabled: deviceSelect.selectedOption.text != "PlayBook"
                        options: [
                            Option {
                                text: qsTr("Latest") + Retranslate.onLocaleOrLanguageChanged
                            },
                            Option {
                                text: "10.2.1"
                            },
                            Option {
                                text: "10.1.0"
                            },
                            Option {
                                text: qsTr("Any") + Retranslate.onLocaleOrLanguageChanged
                            }
                        ]
                    }
                }
                Button {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Bottom
                    text: qsTr("Close") + Retranslate.onLocaleOrLanguageChanged
                    onClicked: settingsDialog.close()
                }
            }
        }
    ]
}
