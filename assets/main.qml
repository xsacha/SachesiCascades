import bb.cascades 1.3

TabbedPane {
    id: root
    showTabsOnActionBar: false
    Menu.definition: TopMenu {}
    Tab {
        objectName: "appTab"
        imageSource: "asset:///world.png"
        title: qsTr("Apps") + Retranslate.onLocaleOrLanguageChanged
        AppWorld {}
    }
    // Search
    Tab {
        objectName: "searchTab"
        title: qsTr("Search") + Retranslate.onLocaleOrLanguageChanged
        imageSource: "asset:///search.png"
        Search {}
    }
}
