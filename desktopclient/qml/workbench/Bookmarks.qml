import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import "../datapanel/catalog" as Catalog
import "../Global.js" as Global

Item {
    id : navigationFoldercreate
    property color background : "white"
    property color listbackground : "white"
    property color listalternate : "#9B9B9E"
    property int defaultHeight : 0
    property var bookmarkModel: mastercatalog.bookmarked

    signal bookmarksChanged()

    Connections {
         target: catalogProperties
         onBookmarksChanged: { bookmarksChanged()}
     }

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

            text : "Bookmarked Catalogs"
        }
    }


    Rectangle {

        id : listRect
        anchors.top : title.bottom
        anchors.bottom: catalogProperties.top
        anchors.bottomMargin: 5
        anchors.topMargin: 5
        width: parent.width
        color : Global.mainbackgroundcolor
        border.width: 1
        border.color: Global.edgecolor

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
                 height : 40
                width : parent.width - 5
                color: currentFolders.currentIndex === index ? "#99CCFF" : index  % 2 == 0 ? Global.alternatecolor2 : Global.alternatecolor1
                focus : true

                Column {
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
                }

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    cursorShape: Qt.ArrowCursor
                    onClicked: {
                        currentFolders.currentIndex = index;
                        catalogProperties.humannametext = displayName
                        catalogProperties.defintiontext = url
                        catalogProperties.descriptiontext = description
                        mastercatalog.selectedBookmark(url)
                        bigthing.changeCatalog(url)

                    }


                }
            }
            Component.onCompleted: {
                var url = mastercatalog.currentUrl
                currentIndex = mastercatalog.selectedBookmark(url)
            }
        }

    }
    Catalog.CatalogProperties{
        id : catalogProperties
        anchors.bottom : parent.bottom
        height : 200
        clip : true
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
