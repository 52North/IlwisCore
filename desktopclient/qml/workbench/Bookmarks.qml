import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import "../datapanel/catalog" as Catalog
import "../Global.js" as Global
import "../controls" as Controls

Column {
    id : navigationFoldercreate
    property color background : "white"
    property color listbackground : "white"
    property color listalternate : "#9B9B9E"
    property int defaultHeight : 0
    property var bookmarkModel: mastercatalog.bookmarked

    signal bookmarksChanged()
    signal bookmarkadded()

//    Connections {
//         target: catalogProperties
//         onBookmarksChanged: { bookmarksChanged()}
//     }

    spacing : 4
    function deleteCurrentBookmark(){
        var index = currentFolders.currentIndex
        mastercatalog.deleteBookmark(index);
        bookmarkModel = mastercatalog.bookmarked

    }

    opacity : 1
    height : defaultHeight
    clip : true
    state : "visible"
    signal catalogChanged

    BorderImage {
        width : parent.width
        height : 24
        id : title
        border { left: 15; top: 0; right: 15; bottom: 0 }
        smooth : true
        source : "../images/headerblue2CS1.png"
        Text {
            x : 10
            y : 5
            font.bold : true

            text : qsTr("Bookmarked Catalogs")
        }
    }


    Rectangle {

        id : listRect
        width: parent.width
        height : parent.height - title.height - addBookmarkButton.height - 20
        color : Global.mainbackgroundcolor
        border.width: 1
        border.color: Global.edgecolor
        radius: 5

        ListView{
            id : currentFolders
            model : bookmarkModel
            width : parent.width - 10
            height : parent.height -5
            x : 5
            y : 5
            clip : true

            signal catalogChanged()

            delegate : Rectangle {
                id : catalogentry
                 height : 40 + catalogProperties.height
                width : parent.width - 5
                color: currentFolders.currentIndex === index ? "#99CCFF" : index  % 2 == 0 ? Global.alternatecolor2 : Global.alternatecolor1
                focus : true

                Column {
                    id : info
                    width : parent.width - 30
                    Text{
                        id : txtlabel
                        text : displayName
                        maximumLineCount: 1
                        y : 3
                        x : 3
                        font.pointSize: 11
                        elide : Text.ElideMiddle
                        width : parent.width
                        height : 20
                        font.family: "Tahoma"
                    }
                    Text {
                        id : pathLable
                        text : url
                        y : 3
                        x : 3
                        font.pointSize: 8
                        elide : Text.ElideMiddle
                        width : parent.width
                        height : 15
                        font.family: "Tahoma"
                        color : "grey"
                    }
                    Catalog.CatalogProperties{
                        id : catalogProperties
                        state : "minimized"
                        clip : true
                    }
                }
                Image {
                    anchors.top : parent.top
                    anchors.bottom : parent.bottom
                    anchors.topMargin: 2
                    anchors.bottomMargin: 2
                    width : 25
                    anchors.right : info.right
                    anchors.rightMargin: -32
                    source : "../images/drop1.png"
                    Image {
                        id : marker
                        height : 16
                        width : 16
                        x : 3
                        anchors.verticalCenter: parent.verticalCenter
                        source : "../images/splithandle.png"
                    }
                    opacity: 0.6
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if ( marker.rotation == 0)    {
                                marker.rotation = 90
                                marker.source = "../images/splithandledark.png"
                                catalogProperties.humannametext = displayName
                                catalogProperties.defintiontext = url
                                catalogProperties.descriptiontext = description
                                catalogProperties.state = "maximized"
                            }else{
                                marker.rotation = 0
                                marker.source = "../images/splithandle.png"
                                catalogProperties.state = "minimized"
                                displayName = catalogProperties.humannametext
                                description = catalogProperties.descriptiontext
                            }
                        }
                    }

                }

                MouseArea {
                    hoverEnabled: true
                    width : parent.width - 30
                    height : 40
                    cursorShape: Qt.ArrowCursor
                    onClicked: {
                        currentFolders.currentIndex = index;

                        mastercatalog.selectedBookmark(url)
                        var filter = "container='" + url + "'"
                        bigthing.changeCatalog(filter,"catalog", url)

                    }


                }
            }
            Component.onCompleted: {
                var url = mastercatalog.currentUrl
                currentIndex = mastercatalog.selectedBookmark(url)
            }
        }

    }
    Row {
        height : 45
        width :215
        anchors.right : parent.right
        anchors.rightMargin: 15

        Action {
            id :addbookmark
            onTriggered: {
                mastercatalog.addBookmark(createCatalogForm.currentFolder)
                bookmarkadded()
            }
        }

        Action {
            id : deleteBookmark
            onTriggered : {
                deleteCurrentBookmark()
            }
        }

        Controls.ActionButton{
            id : removeBookmark
            width : 115
            height : 45
            iconsource: "../images/deletebookmarkCS1.png"
            buttontext :  qsTr("Delete Bookmark");
            action : deleteBookmark

        }
        Controls.ActionButton{
            id : addBookmarkButton

            buttontext : "Add Bookmark"
            iconsource : "../images/addbookmarkCS1.png"
            height : 45
            width :115
            action : addbookmark
            enabled : false
            z : 1
        }


    }
//    Catalog.CatalogProperties{
//        id : catalogProperties
//        anchors.bottom : parent.bottom
//        height : 200
//        clip : true
//    }

    states: [
        State { name: "visible"

            PropertyChanges {
                target: navigationFoldercreate
                z : 0
                opacity : 1
            }
        },
        State {
            name : "invisible"
            PropertyChanges {
                target: navigationFoldercreate
                defaultHeight : height
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation
            { id : opatrans; properties: "opacity"; duration : 500 ; easing.type: Easing.OutCubic
                onRunningChanged: {
                    if ( !opatrans.running ){
                        height = opacity == 0 ? 0 : defaultHeight
                        if ( opacity == 0)
                            z = -1
                    }
                }
            }

        }
    ]
}
