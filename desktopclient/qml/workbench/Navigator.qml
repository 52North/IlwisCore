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
    state : "invisble"

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
        headerImage: "../images/navigatorCS1.png"
        headerText:qsTr("Navigator")
    }

    Action {
        id : changeCatalogContent
        onTriggered : {
            addContainer.checked = !addContainer.checked
            createCatalogForm.state = addContainer.checked ? "maximized" : "minimized"
            createCatalogForm.opacity = 1
        }
    }


    
    Action {
        id : createCatalogLeft
        onTriggered : {
            bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "left")
        }

    }

    Action {
        id : createCatalogRight
        onTriggered : {
            bigthing.newCatalog("container='" + mastercatalog.currentUrl +"'","catalog",mastercatalog.currentUrl, "right")
        }

    }

    Item {
        id : buttonBar
        height : 50
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 3
        width : functionBarHeader.width
        x : 10

        Row {
            id : buttonRow
            anchors.top : parent.top
            anchors.margins: 3
            Controls.ActionButton{
                id : addCatalog
                width : 115
                height : buttonBar.height - 3
                iconsource: "../images/newcatalogCS1.png"
                buttontext :  qsTr("New Catalog Left")
                action : createCatalogLeft

            }
            Controls.ActionButton{
                id : addCatalog2
                width : 115
                height : buttonBar.height - 3
                iconsource: "../images/newcatalogCS1.png"
                buttontext :  qsTr("New Catalog Right")
                action : createCatalogRight

            }

        }

    }
    Row {
        height : 20
        anchors.right : functionBarHeader.right
        anchors.rightMargin: -28
        anchors.top : functionBarHeader.bottom
        anchors.topMargin: 4
        width : 75
        Action {
            id :maxButtons
            onTriggered: {
                createCatalogForm.state = "maximized"
            }
        }

        Action {
            id :minButtons
            onTriggered: {
                createCatalogForm.state = "minimized"
            }
        }
        Button{
            id : full
            height : 20
            width :20
            action : maxButtons
            Image { anchors.centerIn : parent; source: "../images/max1.png" }
        }
        Button{
            id : close
            height : 20
            width :20
            action : minButtons
            Image { anchors.centerIn : parent; source: "../images/min1.png" }
        }
    }
    SplitView {
        width : parent.width - 10
        height : parent.height - buttonBar.height - functionBarHeader.height
        anchors.top: buttonBar.bottom
        anchors.margins: 5
        orientation: Qt.Vertical

        CreateCatalogForm{
            id : createCatalogForm
            width : parent.width - 10
            state : "maximized"
            height : parent.height / 2
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
            background: Global.mainbackgroundcolor
            listbackground: "#FFFEF8"
            listalternate: "#FFFEF8"

        }
    }
}
