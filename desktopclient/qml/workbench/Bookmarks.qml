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
    property color background : uicontext.workbenchBGColor
    property color listbackground : uicontext.workbenchBGColor
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
    clip : true
    state : "visible"
    signal catalogChanged

    BorderImage {
        source: "../images/tab_big_green.png"
        border.left: 5; border.top: 5
        border.right: 5; border.bottom: 5
        width : parent.width
        height : 34
        id : title
        smooth : true
        Row {
            width : parent.width
            height : parent.height
            Image {
                source : "../images/bookmark.png"
            }

            Text {
                x : 10
                y : 5
                font.bold : true
                color : "white"
                text : qsTr("Bookmarked Catalogs")
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }


    Rectangle {

        id : listRect
        width: parent.width
        height : parent.height - title.height - addBookmarkButton.height - 25
        color : Global.mainbackgroundcolor
//        border.width: 1
//        border.color: Global.edgecolor
//        radius: 5

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
                color: currentFolders.currentIndex === index ? "#99CCFF" : index  % 2 == 0 ? uicontext.lightestColor: "white"
                focus : true

                Controls.ToolTip{
                    target : info
                    text : description
                }

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
                    id : marker
                    anchors.top : info.top
                    anchors.topMargin: 12
                    width : 25
                    fillMode : Image.Pad
                    anchors.right : info.right
                    anchors.rightMargin: -32
                    source : "../images/bookmark_close.png"
                    rotation: 90
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if ( marker.rotation == 90)    {
                                marker.state = "open"
                                marker.source = "../images/bookmark_open.png"
                                catalogProperties.humannametext = displayName
                                catalogProperties.defintiontext = url
                                catalogProperties.descriptiontext = description
                                catalogProperties.state = "maximized"
                            }else{
                                marker.state = "closed"
                                marker.source = "../images/bookmark_close.png"
                                catalogProperties.state = "minimized"
                                displayName = catalogProperties.humannametext
                                description = catalogProperties.descriptiontext
                            }
                        }
                    }
                    states: [
                        State { name: "closed"

                            PropertyChanges {
                                target: marker
                                z : 0
                                rotation : 90
                            }
                        },
                        State {
                            name : "open"
                            PropertyChanges {
                                target: marker
                                rotation : 0
                            }
                        }

                    ]
                    transitions: [
                        Transition {
                            NumberAnimation
                            { id : opatrans; properties: "rotation"; duration : 500 ; easing.type: Easing.OutCubic
                            }

                        }
                    ]

                }

                MouseArea {
                    hoverEnabled: true
                    width : parent.width - 30
                    height : 40
                    cursorShape: Qt.ArrowCursor
                    onClicked: {
                        currentFolders.currentIndex = index;
                        mastercatalog.selectedBookmark(url)
                        var filter = mastercatalog.filter(url)//"container='" + url + "'"
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

        Controls.ActionButtonV{
            id : removeBookmark
            width : navigationFoldercreate.width /2
            height : 55
            iconsource: "../images/bookmark_delete.png"
            buttontext :  qsTr("Delete Bookmark");
            action : deleteBookmark

        }
        Controls.ActionButtonV{
            id : addBookmarkButton

            buttontext : "Add Bookmark"
            iconsource : "../images/bookmark_add.png"
            height : 55
            width :navigationFoldercreate.width /2
            action : addbookmark
            enabled : false
            z : 1
        }


    }

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
                //defaultHeight : height
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
                        //height = opacity == 0 ? 0 : defaultHeight
                        if ( opacity == 0)
                            z = -1
                    }
                }
            }

        }
    ]
}
