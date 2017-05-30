import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../controls" as Controls
import "../Global.js" as Global

WorkBenchShifter {
    id : navigatorContainer

    property string startfolder
    state : "invisible"

    signal catalogChanged()

    clip : true

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../images/" + name
        return iconP
    }

    function refreshBookmarks() {
        navCreatePanel.bookmarkModel = mastercatalog.bookmarked
    }

    Connections {
        target: navCreatePanel
        onCatalogChanged: { catalogChanged()}
    }

    Connections {
        target: navCreatePanel
        onBookmarkadded: { refreshBookmarks()}
    }

    Connections {
        target: navCreatePanel
        onBookmarksChanged: { refreshBookmarks()}
    }



    FunctionBarHeader{
        id : functionBarHeader
        headerImage: "../images/locator.png"
        headerText:qsTr("Locator")
    }

    Action {
        id : changeCatalogContent
        onTriggered : {
            addContainer.checked = !addContainer.checked
            createCatalogForm.state = addContainer.checked ? "maximized" : "minimized"
            createCatalogForm.opacity = 1
        }
    }
    
    NavigatorTopButtonBar{
        id : buttonBar
        height : 50
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
    }

    SplitView {
        width : parent.width - 10
        height : parent.height - buttonBar.height - functionBarHeader.height
        anchors.top: buttonBar.bottom
        orientation: Qt.Vertical

        CreateCatalogForm{
            id : createCatalogForm
            width : parent.width - 10
            state : "maximized"
            height : parent.height  / 2
            x : parent.x + 5
            beginfolder: startfolder
        }

        Bookmarks{
            id : navCreatePanel
            width : parent.width - 10
            x : parent.x + 5
            defaultHeight: parent.height - buttonBar.height - functionBarHeader.height - createCatalogForm.height - 15
            anchors.top: createCatalogForm.bottom
            anchors.margins: 2
            opacity : 1
            listalternate: "#FFFEF8"

        }
    }
}
